CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libopus)

if(MSVC)
	set(CMAKE_C_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
	set(CMAKE_CXX_FLAGS "/EHsc /RTCc /RTC1 /MTd /Zp16 /GS /Qpar /MP /wd4244")
else()
	ADD_DEFINITIONS(-O2)
	ADD_DEFINITIONS(-Wno-deprecated-writable-strings)
	ADD_DEFINITIONS(-Wno-write-strings)
	ADD_DEFINITIONS(-Wno-unused-result)
endif()

ADD_DEFINITIONS(-DUSE_ALLOCA=1 -DHAVE_STDINT_H=1 -DOPUS_BUILD -DCUSTOM_MODES=1 -DHAVE_LRINT=1 -DHAVE_LRINTF=1)
#ADD_DEFINITIONS(-DVERSION="")

if(APPLE)
	ADD_DEFINITIONS(-DHAVE_LROUND)
	ADD_DEFINITIONS(-DHAVE_SYS_PARAM_H)
endif()

SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)

include_directories(
	.
	src
	include
	celt
	silk
	silk/float
)

set(SRC_FILES
	src/analysis.c
	src/mlp.c
	src/mlp_data.c
	src/opus.c
	src/opus_compare.c
	src/opus_decoder.c
	src/opus_demo.c
	src/opus_encoder.c
	src/opus_multistream.c
	src/opus_multistream_decoder.c
	src/opus_multistream_encoder.c
	src/repacketizer.c
	src/repacketizer_demo.c
	silk/A2NLSF.c
	silk/ana_filt_bank_1.c
	silk/biquad_alt.c
	silk/bwexpander.c
	silk/bwexpander_32.c
	silk/check_control_input.c
	silk/CNG.c
	silk/code_signs.c
	silk/control_audio_bandwidth.c
	silk/control_codec.c
	silk/control_SNR.c
	silk/debug.c
	silk/dec_API.c
	silk/decode_core.c
	silk/decode_frame.c
	silk/decode_indices.c
	silk/decode_parameters.c
	silk/decode_pitch.c
	silk/decode_pulses.c
	silk/decoder_set_fs.c
	silk/enc_API.c
	silk/encode_indices.c
	silk/encode_pulses.c
	silk/gain_quant.c
	silk/HP_variable_cutoff.c
	silk/init_decoder.c
	silk/init_encoder.c
	silk/inner_prod_aligned.c
	silk/interpolate.c
	silk/lin2log.c
	silk/log2lin.c
	silk/LP_variable_cutoff.c
	silk/LPC_analysis_filter.c
	silk/LPC_inv_pred_gain.c
	silk/NLSF_decode.c
	silk/NLSF_del_dec_quant.c
	silk/NLSF_encode.c
	silk/NLSF_stabilize.c
	silk/NLSF_unpack.c
	silk/NLSF_VQ.c
	silk/NLSF_VQ_weights_laroia.c
	silk/NLSF2A.c
	silk/NSQ.c
	silk/NSQ_del_dec.c
	silk/pitch_est_tables.c
	silk/PLC.c
	silk/process_NLSFs.c
	silk/quant_LTP_gains.c
	silk/resampler.c
	silk/resampler_down2.c
	silk/resampler_down2_3.c
	silk/resampler_private_AR2.c
	silk/resampler_private_down_FIR.c
	silk/resampler_private_IIR_FIR.c
	silk/resampler_private_up2_HQ.c
	silk/resampler_rom.c
	silk/shell_coder.c
	silk/sigm_Q15.c
	silk/sort.c
	silk/stereo_decode_pred.c
	silk/stereo_encode_pred.c
	silk/stereo_find_predictor.c
	silk/stereo_LR_to_MS.c
	silk/stereo_MS_to_LR.c
	silk/stereo_quant_pred.c
	silk/sum_sqr_shift.c
	silk/table_LSF_cos.c
	silk/tables_gain.c
	silk/tables_LTP.c
	silk/tables_NLSF_CB_NB_MB.c
	silk/tables_NLSF_CB_WB.c
	silk/tables_other.c
	silk/tables_pitch_lag.c
	silk/tables_pulses_per_block.c
	silk/VAD.c
	silk/VQ_WMat_EC.c
	celt/bands.c
	celt/celt.c
	celt/celt_decoder.c
	celt/celt_encoder.c
	celt/celt_lpc.c
	celt/cwrs.c
	celt/entcode.c
	celt/entdec.c
	celt/entenc.c
	celt/kiss_fft.c
	celt/laplace.c
	celt/mathops.c
	celt/mdct.c
	celt/modes.c
	celt/pitch.c
	celt/quant_bands.c
	celt/rate.c
	celt/vq.c
)

if(MSVC)
	set(SRC_FILES
		${SRC_FILES}
#		src/share/win_utf8_io/win_utf8_io.c
	)
endif()

set(HEADER_FILES
	include/opus.h
	include/opus_custom.h
	include/opus_defines.h
	include/opus_multistream.h
	include/opus_types.h
)

add_library(libopus ${SRC_FILES} ${HEADER_FILES}
	)
