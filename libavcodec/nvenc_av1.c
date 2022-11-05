/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/internal.h"

#include "avcodec.h"
#include "codec_internal.h"

#include "nvenc.h"

#define OFFSET(x) offsetof(NvencContext, x)
#define VE AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "preset",       "Set the encoding preset",            OFFSET(preset),       AV_OPT_TYPE_INT,   { .i64 = PRESET_P4 }, PRESET_DEFAULT, PRESET_P7, VE, "preset" },
    { "default",      "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_DEFAULT }, 0, 0, VE, "preset" },
    { "slow",         "hq 2 passes",                        0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_SLOW },    0, 0, VE, "preset" },
    { "medium",       "hq 1 pass",                          0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_MEDIUM },  0, 0, VE, "preset" },
    { "fast",         "hp 1 pass",                          0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_FAST },    0, 0, VE, "preset" },
    { "p1",           "fastest (lowest quality)",           0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_P1 },      0, 0, VE, "preset" },
    { "p2",           "faster (lower quality)",             0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_P2 },      0, 0, VE, "preset" },
    { "p3",           "fast (low quality)",                 0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_P3 },      0, 0, VE, "preset" },
    { "p4",           "medium (default)",                   0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_P4 },      0, 0, VE, "preset" },
    { "p5",           "slow (good quality)",                0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_P5 },      0, 0, VE, "preset" },
    { "p6",           "slower (better quality)",            0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_P6 },      0, 0, VE, "preset" },
    { "p7",           "slowest (best quality)",             0,                    AV_OPT_TYPE_CONST, { .i64 = PRESET_P7 },      0, 0, VE, "preset" },
    { "tune",         "Set the encoding tuning info",       OFFSET(tuning_info),  AV_OPT_TYPE_INT,   { .i64 = NV_ENC_TUNING_INFO_HIGH_QUALITY }, NV_ENC_TUNING_INFO_HIGH_QUALITY, NV_ENC_TUNING_INFO_LOSSLESS,  VE, "tune" },
    { "hq",           "High quality",                       0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TUNING_INFO_HIGH_QUALITY },      0, 0, VE, "tune" },
    { "ll",           "Low latency",                        0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TUNING_INFO_LOW_LATENCY },       0, 0, VE, "tune" },
    { "ull",          "Ultra low latency",                  0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY }, 0, 0, VE, "tune" },
    { "lossless",     "Lossless",                           0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TUNING_INFO_LOSSLESS },          0, 0, VE, "tune" },
    { "level",        "Set the encoding level restriction", OFFSET(level),        AV_OPT_TYPE_INT,   { .i64 = NV_ENC_LEVEL_AV1_AUTOSELECT }, NV_ENC_LEVEL_AV1_2, NV_ENC_LEVEL_AV1_AUTOSELECT, VE, "level" },
    { "auto",         "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_AUTOSELECT },  0, 0, VE,  "level" },
    { "2",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_2 },           0, 0, VE,  "level" },
    { "2.0",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_2 },           0, 0, VE,  "level" },
    { "2.1",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_21 },          0, 0, VE,  "level" },
    { "2.2",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_22 },          0, 0, VE,  "level" },
    { "2.3",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_23 },          0, 0, VE,  "level" },
    { "3",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_3 },           0, 0, VE,  "level" },
    { "3.0",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_3 },           0, 0, VE,  "level" },
    { "3.1",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_31 },          0, 0, VE,  "level" },
    { "3.2",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_32 },          0, 0, VE,  "level" },
    { "3.3",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_33 },          0, 0, VE,  "level" },
    { "4",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_4 },           0, 0, VE,  "level" },
    { "4.0",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_4 },           0, 0, VE,  "level" },
    { "4.1",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_41 },          0, 0, VE,  "level" },
    { "4.2",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_42 },          0, 0, VE,  "level" },
    { "4.3",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_43 },          0, 0, VE,  "level" },
    { "5",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_5 },           0, 0, VE,  "level" },
    { "5.0",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_5 },           0, 0, VE,  "level" },
    { "5.1",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_51 },          0, 0, VE,  "level" },
    { "5.2",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_52 },          0, 0, VE,  "level" },
    { "5.3",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_53 },          0, 0, VE,  "level" },
    { "6",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_6 },           0, 0, VE,  "level" },
    { "6.0",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_6 },           0, 0, VE,  "level" },
    { "6.1",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_61 },          0, 0, VE,  "level" },
    { "6.2",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_62 },          0, 0, VE,  "level" },
    { "6.3",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_63 },          0, 0, VE,  "level" },
    { "7",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_7 },           0, 0, VE,  "level" },
    { "7.0",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_7 },           0, 0, VE,  "level" },
    { "7.1",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_71 },          0, 0, VE,  "level" },
    { "7.2",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_72 },          0, 0, VE,  "level" },
    { "7.3",          "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_LEVEL_AV1_73 },          0, 0, VE,  "level" },
    { "tier",         "Set the encoding tier",              OFFSET(tier),         AV_OPT_TYPE_INT,   { .i64 = NV_ENC_TIER_AV1_0 }, NV_ENC_TIER_AV1_0, NV_ENC_TIER_AV1_1, VE, "tier"},
    { "0",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TIER_AV1_0 }, 0, 0, VE, "tier" },
    { "1",            "",                                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TIER_AV1_1 }, 0, 0, VE, "tier" },
    { "rc",           "Override the preset rate-control",   OFFSET(rc),           AV_OPT_TYPE_INT,   { .i64 = -1 },                                  -1, INT_MAX, VE, "rc" },
    { "constqp",      "Constant QP mode",                   0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_PARAMS_RC_CONSTQP },                   0, 0, VE, "rc" },
    { "vbr",          "Variable bitrate mode",              0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_PARAMS_RC_VBR },                       0, 0, VE, "rc" },
    { "cbr",          "Constant bitrate mode",              0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_PARAMS_RC_CBR },                       0, 0, VE, "rc" },
    { "multipass",    "Set the multipass encoding",         OFFSET(multipass),    AV_OPT_TYPE_INT,   { .i64 = NV_ENC_MULTI_PASS_DISABLED },         NV_ENC_MULTI_PASS_DISABLED, NV_ENC_TWO_PASS_FULL_RESOLUTION, VE, "multipass" },
    { "disabled",     "Single Pass",                        0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_MULTI_PASS_DISABLED },         0,                          0,                               VE, "multipass" },
    { "qres",         "Two Pass encoding is enabled where first Pass is quarter resolution",
                                                            0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TWO_PASS_QUARTER_RESOLUTION }, 0,                          0,                               VE, "multipass" },
    { "fullres",      "Two Pass encoding is enabled where first Pass is full resolution",
                                                            0,                    AV_OPT_TYPE_CONST, { .i64 = NV_ENC_TWO_PASS_FULL_RESOLUTION },    0,                          0,                               VE, "multipass" },
    { "highbitdepth", "Enable 10 bit encode for 8 bit input",OFFSET(highbitdepth),AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "tile-rows",    "Number of tile rows to encode with", OFFSET(tile_rows),    AV_OPT_TYPE_INT,   { .i64 = -1 }, -1, NV_MAX_TILE_ROWS_AV1, VE },
    { "tile-columns", "Number of tile columns to encode with", OFFSET(tile_cols), AV_OPT_TYPE_INT,   { .i64 = -1 }, -1, NV_MAX_TILE_COLS_AV1, VE },
    { "surfaces",     "Number of concurrent surfaces",      OFFSET(nb_surfaces),  AV_OPT_TYPE_INT,   { .i64 = 0 }, 0, MAX_REGISTERED_FRAMES, VE },
    { "gpu",          "Selects which NVENC capable GPU to use. First GPU is 0, second is 1, and so on.",
                                                            OFFSET(device),       AV_OPT_TYPE_INT,   { .i64 = ANY_DEVICE }, -2, INT_MAX, VE, "gpu" },
    { "any",          "Pick the first device available",    0,                    AV_OPT_TYPE_CONST, { .i64 = ANY_DEVICE },        0, 0, VE, "gpu" },
    { "list",         "List the available devices",         0,                    AV_OPT_TYPE_CONST, { .i64 = LIST_DEVICES },      0, 0, VE, "gpu" },
    { "delay",        "Delay frame output by the given amount of frames",
                                                            OFFSET(async_depth),  AV_OPT_TYPE_INT,   { .i64 = INT_MAX }, 0, INT_MAX, VE },
    { "rc-lookahead", "Number of frames to look ahead for rate-control",
                                                            OFFSET(rc_lookahead), AV_OPT_TYPE_INT,   { .i64 = 0 }, 0, INT_MAX, VE },
    { "cq",           "Set target quality level (0 to 51, 0 means automatic) for constant quality mode in VBR rate control",
                                                            OFFSET(quality),      AV_OPT_TYPE_FLOAT, { .dbl = 0.}, 0., 51., VE },
    { "init_qpP",     "Initial QP value for P frame",       OFFSET(init_qp_p),    AV_OPT_TYPE_INT,   { .i64 = -1 }, -1, 255, VE },
    { "init_qpB",     "Initial QP value for B frame",       OFFSET(init_qp_b),    AV_OPT_TYPE_INT,   { .i64 = -1 }, -1, 255, VE },
    { "init_qpI",     "Initial QP value for I frame",       OFFSET(init_qp_i),    AV_OPT_TYPE_INT,   { .i64 = -1 }, -1, 255, VE },
    { "qp",           "Constant quantization parameter rate control method",
                                                            OFFSET(cqp),          AV_OPT_TYPE_INT,   { .i64 = -1 }, -1, 255, VE },
    { "qp_cb_offset", "Quantization parameter offset for cb channel",
                                                            OFFSET(qp_cb_offset), AV_OPT_TYPE_INT,   { .i64 = 0 }, -12, 12, VE },
    { "qp_cr_offset", "Quantization parameter offset for cr channel",
                                                            OFFSET(qp_cr_offset), AV_OPT_TYPE_INT,   { .i64 = 0 }, -12, 12, VE },
    { "no-scenecut",  "When lookahead is enabled, set this to 1 to disable adaptive I-frame insertion at scene cuts",
                                                            OFFSET(no_scenecut),  AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "forced-idr",   "If forcing keyframes, force them as IDR frames.",
                                                            OFFSET(forced_idr),   AV_OPT_TYPE_BOOL,  { .i64 = 0 }, -1, 1, VE },
    { "b_adapt",      "When lookahead is enabled, set this to 0 to disable adaptive B-frame decision",
                                                            OFFSET(b_adapt),      AV_OPT_TYPE_BOOL,  { .i64 = 1 }, 0,  1, VE },
    { "spatial-aq",   "set to 1 to enable Spatial AQ",      OFFSET(aq),           AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "temporal-aq",  "set to 1 to enable Temporal AQ",     OFFSET(temporal_aq),  AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "zerolatency",  "Set 1 to indicate zero latency operation (no reordering delay)",
                                                            OFFSET(zerolatency),  AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "nonref_p",     "Set this to 1 to enable automatic insertion of non-reference P-frames",
                                                            OFFSET(nonref_p),     AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "strict_gop",   "Set 1 to minimize GOP-to-GOP rate fluctuations",
                                                            OFFSET(strict_gop),   AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "aq-strength",  "When Spatial AQ is enabled, this field is used to specify AQ strength. AQ strength scale is from 1 (low) - 15 (aggressive)",
                                                            OFFSET(aq_strength),  AV_OPT_TYPE_INT,   { .i64 = 8 }, 1, 15, VE },
    { "weighted_pred","Enable weighted prediction",         OFFSET(weighted_pred),AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "b_ref_mode",   "Use B frames as references",         OFFSET(b_ref_mode),   AV_OPT_TYPE_INT,   { .i64 = -1 }, -1, NV_ENC_BFRAME_REF_MODE_MIDDLE, VE, "b_ref_mode" },
    { "disabled",     "B frames will not be used for reference", 0,               AV_OPT_TYPE_CONST, { .i64 = NV_ENC_BFRAME_REF_MODE_DISABLED }, 0, 0, VE, "b_ref_mode" },
    { "each",         "Each B frame will be used for reference", 0,               AV_OPT_TYPE_CONST, { .i64 = NV_ENC_BFRAME_REF_MODE_EACH }, 0, 0, VE, "b_ref_mode" },
    { "middle",       "Only (number of B frames)/2 will be used for reference", 0,AV_OPT_TYPE_CONST, { .i64 = NV_ENC_BFRAME_REF_MODE_MIDDLE },  0, 0, VE, "b_ref_mode" },
    { "dpb_size",     "Specifies the DPB size used for encoding (0 means automatic)",
                                                            OFFSET(dpb_size),     AV_OPT_TYPE_INT,   { .i64 = 0 }, 0, INT_MAX, VE },
    { "ldkfs",        "Low delay key frame scale; Specifies the Scene Change frame size increase allowed in case of single frame VBV and CBR",
                                                            OFFSET(ldkfs),        AV_OPT_TYPE_INT,   { .i64 = 0 }, 0, UCHAR_MAX, VE },
    { "intra-refresh","Use Periodic Intra Refresh instead of IDR frames",
                                                            OFFSET(intra_refresh),AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "timing-info",  "Include timing info in sequence/frame headers",
                                                            OFFSET(timing_info),  AV_OPT_TYPE_BOOL,  { .i64 = 0 }, 0, 1, VE },
    { "extra_sei",    "Pass on extra SEI data (e.g. a53 cc) to be included in the bitstream",
                                                            OFFSET(extra_sei),    AV_OPT_TYPE_BOOL,  { .i64 = 1 }, 0, 1, VE },
    { "a53cc",        "Use A53 Closed Captions (if available)", OFFSET(a53_cc),   AV_OPT_TYPE_BOOL,  { .i64 = 1 }, 0, 1, VE },
    { "s12m_tc",      "Use timecode (if available)",        OFFSET(s12m_tc),      AV_OPT_TYPE_BOOL,  { .i64 = 1 }, 0, 1, VE },
    { NULL }
};

static const FFCodecDefault defaults[] = {
    { "b", "2M" },
    { "qmin", "-1" },
    { "qmax", "-1" },
    { "qdiff", "-1" },
    { "qblur", "-1" },
    { "qcomp", "-1" },
    { "g", "250" },
    { "bf", "-1" },
    { "refs", "0" },
    { NULL },
};

static const AVClass av1_nvenc_class = {
    .class_name = "av1_nvenc",
    .item_name = av_default_item_name,
    .option = options,
    .version = LIBAVUTIL_VERSION_INT,
};

const FFCodec ff_av1_nvenc_encoder = {
    .p.name         = "av1_nvenc",
    CODEC_LONG_NAME("NVIDIA NVENC av1 encoder"),
    .p.type         = AVMEDIA_TYPE_VIDEO,
    .p.id           = AV_CODEC_ID_AV1,
    .init           = ff_nvenc_encode_init,
    FF_CODEC_RECEIVE_PACKET_CB(ff_nvenc_receive_packet),
    .close          = ff_nvenc_encode_close,
    .flush          = ff_nvenc_encode_flush,
    .priv_data_size = sizeof(NvencContext),
    .p.priv_class   = &av1_nvenc_class,
    .defaults       = defaults,
    .p.pix_fmts     = ff_nvenc_pix_fmts,
    .p.capabilities = AV_CODEC_CAP_DELAY | AV_CODEC_CAP_HARDWARE |
                      AV_CODEC_CAP_ENCODER_FLUSH | AV_CODEC_CAP_DR1,
    .caps_internal  = FF_CODEC_CAP_NOT_INIT_THREADSAFE |
                      FF_CODEC_CAP_INIT_CLEANUP,
    .p.wrapper_name = "nvenc",
    .hw_configs     = ff_nvenc_hw_configs,
};
