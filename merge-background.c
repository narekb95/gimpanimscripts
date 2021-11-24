#include <libgimp/gimp.h>

#include <stdio.h>
#include <stdlib.h>
void itoa(int val, char* buffer, int size){
	static char buf[32] = {0};
	int n = 0;
	while(val)
	{
		buffer[n++]=(val%10 + '0');
		val/=10;
	}
	for(int i = 0; i < n/2; i++)
	{
		buffer[i] ^= buffer[n-i-1];
		buffer[n-i-1] ^= buffer[i];
		buffer[i] ^= buffer[n-i-1];
	}
	buffer[n] = '\0';
}
	

static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure (
    "plug-in-merge-background",
    "merge background",
    "Merges selected layer to all layers",
    "Narek Bojikian",
    "Copyright Narek Bojikian",
    "2021",
    "_Merge Background...",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register ("plug-in-merge-background",
                             "<Image>/Filters/Misc");
}

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  int numlayers;
  int image = param[1].data.d_image;
  int *layers = gimp_image_get_layers(image, &numlayers);
  int selectedLayerId = gimp_image_get_active_layer(image);
  int currentState = gimp_item_get_visible (selectedLayerId);
  //GimpDrawable *selectedLayer = gimp_drawable_get (selectedLayerId);

  for(int i = 0; i < numlayers; i++)
  {
	  gimp_item_set_visible(layers[i], 0);
  }

  for(int i = 0; i < numlayers; i++)
  {
          if(layers[i] == selectedLayerId)
          {
        	  continue;
          }
	  gimp_item_set_visible(selectedLayerId, 1);
	  int newid = gimp_layer_copy (selectedLayerId);
	  gimp_image_insert_layer (image, newid, 0, -1);
	  gimp_item_set_visible(selectedLayerId, 0);
	  gimp_item_set_visible(layers[i], 1);
	  //gimp_item_set_visible(newid, 1);
	  int outid = gimp_image_merge_visible_layers (image,
			  GIMP_EXPAND_AS_NECESSARY);
	  gimp_item_set_visible(outid, 0);
	  layers[i] = outid;
  }
  for(int i = 0; i < numlayers; i++)
  {
          if(layers[i] == selectedLayerId)
          {
        	  continue;
          }
	  gimp_item_set_visible(layers[i], 1);
  }
  gimp_displays_flush ();
}
