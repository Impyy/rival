#include <ui.h>
#include <stdio.h>

#include "gui.h"
#include "util.h"

static uiWindow *window = NULL;
static struct gui_model *model = NULL;

static int on_closing(uiWindow *w, void *data)
{
	uiQuit();

	if (model->handler_close) {
		model->handler_close();
	}

	return 1;
}

static int on_should_quit(void *data)
{
	uiWindow *window = uiWindow(data);
	uiControlDestroy(uiControl(window));
	return 1;
}

static void on_apply_clicked(uiButton *button, void *data)
{
	if (model->handler_apply) {
		model->handler_apply();
	}
}

static void on_save_clicked(uiButton *button, void *data)
{
	if (model->handler_save) {
		model->handler_save();
	}
}

static void on_close_clicked(uiButton *button, void *data)
{
	on_closing(window, NULL);
}

static void on_preview_toggled(uiCheckbox *button, void *data)
{
	*model->enable_preview = uiCheckboxChecked(button);
}

static void on_mode_selected(uiCombobox *box, void *data)
{
	int mode = uiComboboxSelected(box);
	*model->light_mode = mode;

	if (!*model->enable_preview) {
		return;
	}

	if (model->handler_mode) {
		model->handler_mode(mode);
	}
}

static void on_dpi_selected(uiCombobox *box, void *data)
{
	int dpi = uiComboboxSelected(box);
	*model->dpi = dpi;

	if (!*model->enable_preview) {
		return;
	}

	if (model->handler_dpi) {
		model->handler_dpi(RIVAL_DPI_PRESET_FIRST, dpi);
	}
}

static void on_rate_selected(uiCombobox *box, void *data)
{
	int rate = uiComboboxSelected(box);
	*model->rate = rate;

	if (!*model->enable_preview) {
		return;
	}

	if (model->handler_rate) {
		model->handler_rate(rate);
	}
}

static void on_color_changed(uiColorButton *button, void *data)
{
	double r, g, b, a = {0};
	uiColorButtonColor(button, &r, &g, &b, &a);

	*model->color_r = color_to_byte(r);
	*model->color_g = color_to_byte(g);
	*model->color_b = color_to_byte(b);

	if (!*model->enable_preview) {
		return;
	}

	if (model->handler_color) {
		model->handler_color(*model->color_r, *model->color_g, *model->color_b);
	}
}

bool gui_setup()
{
	uiInitOptions options = {0};
	const char *error = uiInit(&options);
	if (error != NULL) {
		printf("error initializing libui: %s\n", error);
		uiFreeInitError(error);
		return false;
	}

	uiButton *saveButton = uiNewButton("Save");
	uiButtonOnClicked(saveButton, on_save_clicked, NULL);
	uiButton *applyButton = uiNewButton("Apply");
	uiButtonOnClicked(applyButton, on_apply_clicked, NULL);
	uiButton *closeButton = uiNewButton("Close");
	uiButtonOnClicked(closeButton, on_close_clicked, NULL);
	uiCheckbox *previewCheckbox = uiNewCheckbox("Enable preview");
	uiCheckboxSetChecked(previewCheckbox, *model->enable_preview);
	uiCheckboxOnToggled(previewCheckbox, on_preview_toggled, NULL);

	uiBox *buttonBox = uiNewHorizontalBox();
	uiBoxSetPadded(buttonBox, true);
	uiBoxAppend(buttonBox, uiControl(saveButton), false);
	uiBoxAppend(buttonBox, uiControl(applyButton), false);
	uiBoxAppend(buttonBox, uiControl(closeButton), false);
	uiBoxAppend(buttonBox, uiControl(previewCheckbox), false);

	double r = color_to_double(*model->color_r);
	double g = color_to_double(*model->color_g);
	double b = color_to_double(*model->color_b);

	uiLabel *colorLabel = uiNewLabel("Color");
	uiColorButton *colorButton = uiNewColorButton();
	uiColorButtonOnChanged(colorButton, on_color_changed, NULL);
	uiColorButtonSetColor(colorButton, r, g, b, 255.0);
	uiLabel *modeLabel = uiNewLabel("Mode");
	uiCombobox *modeCombobox = uiNewCombobox();
	uiComboboxOnSelected(modeCombobox, on_mode_selected, NULL);
	uiComboboxAppend(modeCombobox, "Steady");
	uiComboboxAppend(modeCombobox, "Breathe (slow)");
	uiComboboxAppend(modeCombobox, "Breathe (medium)");
	uiComboboxAppend(modeCombobox, "Breathe (fast)");
	uiComboboxAppend(modeCombobox, "Off");
	uiComboboxSetSelected(modeCombobox, *model->light_mode);

	uiLabel *dpiLabel = uiNewLabel("Sensitivity in DPI");
	uiCombobox *dpiCombobox = uiNewCombobox();
	uiComboboxOnSelected(dpiCombobox, on_dpi_selected, NULL);
	uiComboboxAppend(dpiCombobox, "4000");
	uiComboboxAppend(dpiCombobox, "2000");
	uiComboboxAppend(dpiCombobox, "1750");
	uiComboboxAppend(dpiCombobox, "1500");
	uiComboboxAppend(dpiCombobox, "1250");
	uiComboboxAppend(dpiCombobox, "1000");
	uiComboboxAppend(dpiCombobox, "500");
	uiComboboxAppend(dpiCombobox, "250");
	uiComboboxSetSelected(dpiCombobox, *model->dpi);

	uiLabel *rateLabel = uiNewLabel("Polling rate in Hz");
	uiCombobox *rateCombobox = uiNewCombobox();
	uiComboboxOnSelected(rateCombobox, on_rate_selected, NULL);
	uiComboboxAppend(rateCombobox, "1000");
	uiComboboxAppend(rateCombobox, "500");
	uiComboboxAppend(rateCombobox, "250");
	uiComboboxAppend(rateCombobox, "125");
	uiComboboxSetSelected(rateCombobox, *model->rate);

	uiBox *sensBox = uiNewVerticalBox();
	uiBoxSetPadded(sensBox, true);
	uiBoxAppend(sensBox, uiControl(dpiLabel), false);
	uiBoxAppend(sensBox, uiControl(dpiCombobox), false);
	uiBoxAppend(sensBox, uiControl(rateLabel), false);
	uiBoxAppend(sensBox, uiControl(rateCombobox), false);
	/*uiForm *sensBox = uiNewForm();
	uiFormSetPadded(sensBox, true);
	uiFormAppend(sensBox, "DPI", uiControl(dpiCombobox), false);
	uiFormAppend(sensBox, "Polling rate", uiControl(rateCombobox), false);*/

	uiBox *lightBox = uiNewVerticalBox();
	uiBoxSetPadded(lightBox, true);
	uiBoxAppend(lightBox, uiControl(colorLabel), false);
	uiBoxAppend(lightBox, uiControl(colorButton), false);
	uiBoxAppend(lightBox, uiControl(modeLabel), false);
	uiBoxAppend(lightBox, uiControl(modeCombobox), false);
	/*uiForm *lightBox = uiNewForm();
	uiFormSetPadded(lightBox, true);
	uiFormAppend(lightBox, "Color", uiControl(colorButton), false);
	uiFormAppend(lightBox, "Mode", uiControl(modeCombobox), false);*/

	uiGroup *sensGroup = uiNewGroup("Controls");
	uiGroupSetMargined(sensGroup, true);
	uiGroupSetChild(sensGroup, uiControl(sensBox));

	uiGroup *lightGroup = uiNewGroup("Lights");
	uiGroupSetMargined(lightGroup, true);
	uiGroupSetChild(lightGroup, uiControl(lightBox));

	uiBox *firstBox = uiNewHorizontalBox();
	uiBoxSetPadded(firstBox, true);
	uiBoxAppend(firstBox, uiControl(sensGroup), true);
	uiBoxAppend(firstBox, uiControl(lightGroup), true);

	uiBox *mainBox = uiNewVerticalBox();
	uiBoxSetPadded(firstBox, true);
	uiBoxAppend(mainBox, uiControl(firstBox), true);
	uiBoxAppend(mainBox, uiControl(buttonBox), false);

	window = uiNewWindow("rival", 400, 200, true);
	uiWindowSetMargined(window, true);
	uiWindowOnClosing(window, on_closing, NULL);
	uiWindowSetChild(window, uiControl(mainBox));
	uiOnShouldQuit(on_should_quit, window);

	uiControlShow(uiControl(window));
	return true;
}

void *gui_main(void *data)
{
	uiMain();
	return NULL;
}

void gui_queue(void (*f)(void *data), void *data)
{
	uiQueueMain(f, data);
}

void gui_set_title(const char *title)
{
	uiWindowSetTitle(window, title);
}

void gui_set_model(struct gui_model *m)
{
	model = m;
}
