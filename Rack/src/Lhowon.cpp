#include "plugin.hpp"
#include "Lhowon.h"

const float SCALE = 3.77953;

inline math::Vec mm2pxZ(math::Vec mm) {
	return mm.mult(SVG_DPI / MM_PER_IN / SCALE);
}

struct Lhowon : Module {

	enum ParamId {
		KNOB_1_PARAM,
		KNOB_2_PARAM,
		KNOB_3_PARAM,
		KNOB_4_PARAM,
		KNOB_5_PARAM,
		KNOB_6_PARAM,
		KNOB_7_PARAM,
		BUTTON_FSU_PARAM,
		SWITCH_ABC_PARAM,
		SWITCH_XYZ_PARAM,
		PARAMS_LEN
	};
        
	enum InputId {
		CV_1_INPUT,
		CV_3_INPUT,
		CV_5_INPUT,
		CV_7_INPUT,
		CV_2_INPUT,
		CV_4_INPUT,
		CV_6_INPUT,
		CV_FSU_INPUT,
		IN_1_INPUT,
		IN_2_INPUT,
		INPUTS_LEN
	};
    
	enum OutputId {
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LIGHT_1_LIGHT,
		LIGHT_2_LIGHT,
		LIGHT_3_LIGHT,
		LIGHT_4_LIGHT,
		LIGHTS_LEN
	};

    planetbosch::Lhowon core;  
    //SchmittTrigger schmittTrigger;
    float in[2];
    float out[2];

	Lhowon() {
        core.Init(48000.f);
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(CV_1_INPUT, "CV 1");
		configInput(CV_2_INPUT, "CV 2");
		configInput(CV_3_INPUT, "CV 3");
		configInput(CV_4_INPUT, "CV 4");
		configInput(CV_5_INPUT, "CV 5");
		configInput(CV_6_INPUT, "CV 6");
		configInput(CV_7_INPUT, "CV 7");
		configInput(CV_FSU_INPUT, "FSU");
		configInput(IN_1_INPUT, "1");
		configInput(IN_2_INPUT, "2");
		configOutput(OUT_1_OUTPUT, "1");
		configOutput(OUT_2_OUTPUT, "2");
		configButton(BUTTON_FSU_PARAM);
		configParam(KNOB_1_PARAM, 0.f, 1.f, 0.f, "1");
		configParam(KNOB_2_PARAM, 0.f, 1.f, 0.f, "2");
		configParam(KNOB_3_PARAM, 0.f, 1.f, 0.f, "3");
		configParam(KNOB_4_PARAM, 0.f, 1.f, 0.f, "4");
		configParam(KNOB_5_PARAM, 0.f, 1.f, 0.f, "5");
		configParam(KNOB_6_PARAM, 0.f, 1.f, 0.f, "6");
		configParam(KNOB_7_PARAM, 0.f, 1.f, 0.f, "7");
		configSwitch(SWITCH_ABC_PARAM, -1.f, 1.f, 0.f, "ABC", {"A", "B", "C"});
		configSwitch(SWITCH_XYZ_PARAM, -1.f, 1.f, 0.f, "XYZ", {"X", "Y", "Z"});
	}

	void process(const ProcessArgs& args) override {
        in[0] = inputs[IN_1_INPUT].getVoltage() / 5.f;
        if (inputs[IN_2_INPUT].isConnected()) {
            in[1] = inputs[IN_2_INPUT].getVoltage() / 5.f;
        } else {
            in[1] = in[0];
        }
        core.Update(
            clamp((inputs[CV_1_INPUT].getVoltage() / 10.f) + params[KNOB_1_PARAM].getValue(), 0.f, 1.f),
            clamp((inputs[CV_2_INPUT].getVoltage() / 10.f) + params[KNOB_2_PARAM].getValue(), 0.f, 1.f),
            clamp((inputs[CV_3_INPUT].getVoltage() / 10.f) + params[KNOB_3_PARAM].getValue(), 0.f, 1.f),
            clamp((inputs[CV_4_INPUT].getVoltage() / 10.f) + params[KNOB_4_PARAM].getValue(), 0.f, 1.f),
            clamp((inputs[CV_5_INPUT].getVoltage() / 10.f) + params[KNOB_5_PARAM].getValue(), 0.f, 1.f),
            clamp((inputs[CV_6_INPUT].getVoltage() / 10.f) + params[KNOB_6_PARAM].getValue(), 0.f, 1.f),
            clamp((inputs[CV_7_INPUT].getVoltage() / 10.f) + params[KNOB_7_PARAM].getValue(), 0.f, 1.f),
            false
            //schmittTrigger.process((inputs[CV_FSU_INPUT].getVoltage() / 10.f) + params[BUTTON_FSU_PARAM].getValue());
        );
        core.Process(in[0], in[1], out);
        outputs[OUT_1_OUTPUT].setVoltage(out[0] * 5.f);
        outputs[OUT_2_OUTPUT].setVoltage(out[1] * 5.f);
	}
};


struct LhowonWidget : ModuleWidget {
	LhowonWidget(Lhowon* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Lhowon.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<NKK>(mm2pxZ(Vec(30.624, 218.832)), module, Lhowon::SWITCH_ABC_PARAM));
		addParam(createParamCentered<NKK>(mm2pxZ(Vec(30.624, 254.832)), module, Lhowon::SWITCH_XYZ_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2pxZ(Vec(28.224, 70.032)), module, Lhowon::KNOB_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2pxZ(Vec(28.224, 149.232)), module, Lhowon::KNOB_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2pxZ(Vec(93.984, 108.432)), module, Lhowon::KNOB_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2pxZ(Vec(93.984, 186.432)), module, Lhowon::KNOB_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2pxZ(Vec(162.624, 70.032)), module, Lhowon::KNOB_5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2pxZ(Vec(162.624, 149.232)), module, Lhowon::KNOB_6_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2pxZ(Vec(162.624, 228.432)), module, Lhowon::KNOB_7_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2pxZ(Vec(97.824, 262.032)), module, Lhowon::BUTTON_FSU_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(18.624, 314.832)), module, Lhowon::CV_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(18.624, 367.632)), module, Lhowon::CV_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(69.024, 314.832)), module, Lhowon::CV_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(69.024, 367.632)), module, Lhowon::CV_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(119.424, 314.832)), module, Lhowon::CV_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(119.424, 367.632)), module, Lhowon::CV_6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(167.424, 314.832)), module, Lhowon::CV_7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(167.424, 367.632)), module, Lhowon::CV_FSU_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(18.624, 420.432)), module, Lhowon::IN_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2pxZ(Vec(69.024, 420.432)), module, Lhowon::IN_2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2pxZ(Vec(119.424, 420.432)), module, Lhowon::OUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2pxZ(Vec(167.424, 420.432)), module, Lhowon::OUT_2_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2pxZ(Vec(64.224, 74.832)), module, Lhowon::LIGHT_1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2pxZ(Vec(83.424, 74.832)), module, Lhowon::LIGHT_2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2pxZ(Vec(109.824, 74.832)), module, Lhowon::LIGHT_3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2pxZ(Vec(129.024, 74.832)), module, Lhowon::LIGHT_4_LIGHT));
	}
};


Model* modelLhowon = createModel<Lhowon, LhowonWidget>("Lhowon");
