#include <ui.h>
#include <stdio.h>

#include "gui.h"
#include "util.h"

static bool enable_preview = true;
static uiWindow *window = NULL;

static gui_handler_color *handler_color = NULL;
static gui_handler_mode *handler_mode = NULL;
static gui_handler_dpi *handler_dpi = NULL;
static gui_handler_rate *handler_rate = NULL;
static gui_handler_close *handler_close = NULL;

static int on_closing(uiWindow *w, void *data)
{
	uiQuit();

	if (handler_close) {
		handler_close();
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

}

static void on_close_clicked(uiButton *button, void *data)
{
	on_closing(window, NULL);
}

static void on_preview_toggled(uiCheckbox *button, void *data)
{
	enable_preview = uiCheckboxChecked(button);
}

static void on_mode_selected(uiCombobox *box, void *data)
{
	if (!enable_preview) {
		return;
	}

	if (handler_mode) {
		int mode = uiComboboxSelected(box);

		// map to RIVAL_LIGHT_MODE by adding 1
		handler_mode(mode + 1);
	}
}

static void on_dpi_selected(uiCombobox *box, void *data)
{
	if (!enable_preview) {
		return;
	}

	if (handler_dpi) {
		int dpi = uiComboboxSelected(box);

		// map to RIVAL_DPI by adding 1
		handler_dpi(RIVAL_DPI_PRESET_FIRST, dpi + 1);
	}
}

static void on_rate_selected(uiCombobox *box, void *data)
{
	if (!enable_preview) {
		return;
	}

	if (handler_rate) {
		int rate = uiComboboxSelected(box);

		// map to RIVAL_RATE by adding 1
		handler_rate(rate + 1);
	}
}

static void on_color_changed(uiColorButton *button, void *data)
{
	if (!enable_preview) {
		return;
	}

	double r, g, b, a = {0};
	uiColorButtonColor(button, &r, &g, &b, &a);

	r = convert_color(r);
	g = convert_color(g);
	b = convert_color(b);

	if (handler_color) {
		handler_color(r, g, b);
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

	uiButton *applyButton = uiNewButton("Apply");
	uiButtonOnClicked(applyButton, on_apply_clicked, NULL);
	uiButton *closeButton = uiNewButton("Close");
	uiButtonOnClicked(closeButton, on_close_clicked, NULL);
	uiCheckbox *previewCheckbox = uiNewCheckbox("Enable preview");
	uiCheckboxSetChecked(previewCheckbox, enable_preview);
	uiCheckboxOnToggled(previewCheckbox, on_preview_toggled, NULL);

	uiBox *buttonBox = uiNewHorizontalBox();
	uiBoxSetPadded(buttonBox, true);
	uiBoxAppend(buttonBox, uiControl(applyButton), false);
	uiBoxAppend(buttonBox, uiControl(closeButton), false);
	uiBoxAppend(buttonBox, uiControl(previewCheckbox), false);

	uiLabel *colorLabel = uiNewLabel("Color");
	uiColorButton *colorButton = uiNewColorButton();
	uiColorButtonOnChanged(colorButton, on_color_changed, NULL);
	uiLabel *modeLabel = uiNewLabel("Mode");
	uiCombobox *modeCombobox = uiNewCombobox();
	uiComboboxOnSelected(modeCombobox, on_mode_selected, NULL);
	uiComboboxAppend(modeCombobox, "Steady");
	uiComboboxAppend(modeCombobox, "Breathe (slow)");
	uiComboboxAppend(modeCombobox, "Breathe (medium)");
	uiComboboxAppend(modeCombobox, "Breathe (fast)");
	uiComboboxAppend(modeCombobox, "Off");

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

	uiLabel *rateLabel = uiNewLabel("Polling rate in Hz");
	uiCombobox *rateCombobox = uiNewCombobox();
	uiComboboxOnSelected(rateCombobox, on_rate_selected, NULL);
	uiComboboxAppend(rateCombobox, "1000");
	uiComboboxAppend(rateCombobox, "500");
	uiComboboxAppend(rateCombobox, "250");
	uiComboboxAppend(rateCombobox, "125");

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

	window = uiNewWindow("rival", 500, 500, true);
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

void gui_register_handler_color(gui_handler_color *h)
{
	handler_color = h;
}

void gui_register_handler_mode(gui_handler_mode *h)
{
	handler_mode = h;
}

void gui_register_handler_dpi(gui_handler_dpi *h)
{
	handler_dpi = h;
}

void gui_register_handler_rate(gui_handler_rate *h)
{
	handler_rate = h;
}

void gui_register_handler_close(gui_handler_close *h)
{
	handler_close = h;
}
