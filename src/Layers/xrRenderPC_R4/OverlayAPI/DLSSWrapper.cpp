#include "stdafx.h"

#include "DLSSWrapper.h"

DLSSWrapper g_DLSSWrapper;

extern ENGINE_API u32 ps_render_scale_preset;
extern ENGINE_API float ps_render_scale;

u32 DLSSWrapper::GetOptimalPresetForScale(float scale)
{
	if (ps_render_scale_preset == 5)
	{
		if (scale >= 0.9f)
		{
			return 0;
		}
		else if (scale >= 0.7f)
		{
			return 1;
		}
		else if (scale >= 0.6f)
		{
			return 2;
		}
		else if (scale >= 0.5f)
		{
			return 3;
		}
		else
		{
			return 4;
		}
	}
	
	return ps_render_scale_preset;
}

void DLSSWrapper::Create()
{
	
}

bool DLSSWrapper::GetRenderScale(float& RenderScale)
{
	
		return false;
	
}

void DLSSWrapper::Resize(const ContextParameters& Parameters)
{
	PROF_EVENT("DLSSWrapper::Resize");

	if (!Created)
	{
		return;
	}

#ifdef IXR_X64
	// Устанавливаем пресет для выбранного режима качества
	u32 PresetID = GetOptimalPresetForScale(ps_render_scale);

	NVSDK_NGX_PerfQuality_Value PerfQualityValue = NVSDK_NGX_PerfQuality_Value_DLAA;
	shared_str RenderPreset = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA;

	switch (PresetID)
	{
		case 4:
		{
			PerfQualityValue = NVSDK_NGX_PerfQuality_Value_UltraPerformance;
			RenderPreset = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_UltraPerformance;
			break;
		}
		case 3:
		{
			PerfQualityValue = NVSDK_NGX_PerfQuality_Value_MaxPerf;
			RenderPreset = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Performance;
			break;
		}
		case 2:
		{
			PerfQualityValue = NVSDK_NGX_PerfQuality_Value_Balanced;
			RenderPreset = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Balanced;
			break;
		}
		case 1:
		{
			PerfQualityValue = NVSDK_NGX_PerfQuality_Value_MaxQuality;
			RenderPreset = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Quality;
			break;
		}
		default:
		{
			PerfQualityValue = NVSDK_NGX_PerfQuality_Value_DLAA;
			RenderPreset = NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA;
			break;
		}
	}

	NgxParameters->Set(*RenderPreset, static_cast<int>(NVSDK_NGX_DLSS_Hint_Render_Preset_K));

	Msg("* Resize DLSSWrapper Render Preset [%s]", *RenderPreset);

	NVSDK_NGX_DLSS_Create_Params DLSSCreateParams = {};

	DLSSCreateParams.Feature.InWidth = Parameters.renderSize.x;
	DLSSCreateParams.Feature.InHeight = Parameters.renderSize.y;

	DLSSCreateParams.Feature.InTargetWidth = Parameters.displaySize.x;
	DLSSCreateParams.Feature.InTargetHeight = Parameters.displaySize.y;

	DLSSCreateParams.Feature.InPerfQualityValue = PerfQualityValue;
	DLSSCreateParams.InFeatureCreateFlags = 0;

	DLSSCreateParams.InFeatureCreateFlags |= NVSDK_NGX_DLSS_Feature_Flags_IsHDR;
	DLSSCreateParams.InFeatureCreateFlags |= NVSDK_NGX_DLSS_Feature_Flags_MVLowRes;
	DLSSCreateParams.InFeatureCreateFlags |= NVSDK_NGX_DLSS_Feature_Flags_AutoExposure;

	NVSDK_NGX_Result Result = NGX_D3D11_CREATE_DLSS_EXT(RContext, &Handle, NgxParameters, &DLSSCreateParams);

	if (Result != NVSDK_NGX_Result_Success)
	{
		Msg("! NGX_D3D11_CREATE_DLSS_EXT not valid. Need use FSR");
		Created = false;
		return;
	}
#endif
}

void DLSSWrapper::Destroy() 
{
#ifdef IXR_X64
	if (Handle != nullptr)
	{
		NVSDK_NGX_D3D11_ReleaseFeature(Handle);
		Handle = nullptr;
	}

	if (NgxParameters != nullptr)
	{
		NVSDK_NGX_D3D11_DestroyParameters(NgxParameters);
		NgxParameters = nullptr;
	}

	if (DLSSInited)
	{
		NVSDK_NGX_D3D11_Shutdown1(nullptr);
		DLSSInited = false;
	}

	Created = false;	
#endif
}

bool DLSSWrapper::Draw(const DrawParameters& params)
{

		return false;

}

DLSSWrapper::~DLSSWrapper()
{
	Destroy();
}
