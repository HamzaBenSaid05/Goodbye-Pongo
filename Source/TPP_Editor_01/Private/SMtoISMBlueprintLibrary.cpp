#include "SMtoISMBlueprintLibrary.h"
#include "SMtoISMConverter.h"

void USMtoISMBlueprintLibrary::ConvertStaticMeshToISM(
	const FSMtoISMSettingsBP Settings,
	FSMtoISMResultBP& Result)
{
	FSMtoISMSettings Cfg;
	Cfg.bReadFromSource      = Settings.bReadFromSource;
	Cfg.bDeleteSourceActors  = Settings.bDeleteSourceActors;
	Cfg.bCastShadow          = Settings.bCastShadow;
	Cfg.bCastDynamicShadow   = Settings.bCastDynamicShadow;
	Cfg.bCastStaticShadow    = Settings.bCastStaticShadow;
	Cfg.bCastContactShadow   = Settings.bCastContactShadow;
	Cfg.bSelfShadowOnly      = Settings.bSelfShadowOnly;
	Cfg.bReceivesDecals      = Settings.bReceivesDecals;
	Cfg.bRenderCustomDepth   = Settings.bRenderCustomDepth;
	Cfg.CustomDepthStencil   = Settings.CustomDepthStencil;
	Cfg.BoundsScale          = Settings.BoundsScale;
	Cfg.ForcedLOD            = Settings.ForcedLOD;
	Cfg.MinLOD               = Settings.MinLOD;
	Cfg.MaxDrawDistance      = Settings.MaxDrawDistance;
	Cfg.bOverrideLightmapRes = Settings.bOverrideLightmapRes;
	Cfg.OverrideLightmapRes  = Settings.OverrideLightmapRes;
	Cfg.LightmapUVChannel    = Settings.LightmapUVChannel;
	Cfg.UVTileU              = Settings.UVTileU;
	Cfg.UVTileV              = Settings.UVTileV;

	FSMtoISMResult R = FSMtoISMConverter::Convert(Cfg);

	Result.ISMActorsCreated = R.ISMActorsCreated;
	Result.SourcesProcessed = R.SourcesProcessed;
	Result.Log              = R.Log;
}
