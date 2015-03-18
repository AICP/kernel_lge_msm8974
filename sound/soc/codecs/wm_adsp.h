/*
 * wm_adsp.h  --  Wolfson ADSP support
 *
 * Copyright 2012 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __WM_ADSP_H
#define __WM_ADSP_H

#include <linux/circ_buf.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/compress_driver.h>

#include "wmfw.h"

struct regulator;

struct wm_adsp_region {
	int type;
	unsigned int base;
};

struct wm_adsp_alg_region {
	struct list_head list;
	unsigned int alg;
	int type;
	unsigned int base;
	size_t len;
};

struct wm_adsp_buffer_region {
	unsigned int offset_samps;
	unsigned int cumulative_samps;
	unsigned int mem_type;
	unsigned int base_addr;
};

struct wm_adsp {
	const char *part;
	int num;
	int type;
	struct device *dev;
	struct regmap *regmap;

	int base;

	struct list_head alg_regions;

	int fw_id;

	const struct wm_adsp_region *mem;
	int num_mems;

	int fw;
	bool running;

	struct regulator *dvfs;

	struct wm_coeff *wm_coeff;

	u32 host_buf_ptr;
	u32 low_water_mark;

	int sample_size;
	u32 *raw_capt_buf;
	int raw_buf_samps;
	struct circ_buf capt_buf;
	int capt_buf_size;
	struct wm_adsp_buffer_region *host_regions;
	bool buffer_drain_pending;
};

#define WM_ADSP1(wname, num) \
	{ .id = snd_soc_dapm_pga, .name = wname, .reg = SND_SOC_NOPM, \
	.shift = num, .event = wm_adsp1_event, \
	.event_flags = SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD }

#define WM_ADSP2(wname, num) \
{	.id = snd_soc_dapm_pga, .name = wname, .reg = SND_SOC_NOPM, \
	.shift = num, .event = wm_adsp2_event, \
	.event_flags = SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD }

extern const struct snd_kcontrol_new wm_adsp_fw_controls[];

int wm_adsp2_init(struct wm_adsp *adsp, bool dvfs);
int wm_adsp1_event(struct snd_soc_dapm_widget *w,
		   struct snd_kcontrol *kcontrol, int event);
int wm_adsp2_event(struct snd_soc_dapm_widget *w,
		   struct snd_kcontrol *kcontrol, int event);

extern bool wm_adsp_compress_supported(const struct wm_adsp* adsp,
				       const struct snd_compr_stream* stream);
extern bool wm_adsp_format_supported(const struct wm_adsp *adsp,
				     const struct snd_compr_stream *stream,
				     const struct snd_compr_params *params);
extern void wm_adsp_get_caps(const struct wm_adsp *adsp,
			     const struct snd_compr_stream *stream,
			     struct snd_compr_caps *caps);

extern int wm_adsp_stream_alloc(struct wm_adsp* adsp,
				const struct snd_compr_params *params);
extern int wm_adsp_stream_free(struct wm_adsp* adsp);
extern int wm_adsp_stream_start(struct wm_adsp *adsp);
extern int wm_adsp_stream_capture(struct wm_adsp *adsp);
extern int wm_adsp_stream_read(struct wm_adsp *adsp, char __user *buf,
			       size_t count);
extern int wm_adsp_stream_avail(const struct wm_adsp *adsp);

#endif
