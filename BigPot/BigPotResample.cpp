#include "BigPotResample.h"


BigPotResample::BigPotResample()
{
}


BigPotResample::~BigPotResample()
{
}

int BigPotResample::convert(AVCodecContext* codecCtx, AVFrame* frame, int out_sample_format, int out_sample_rate, int out_channels, uint8_t* out_buf)
{
    SwrContext* swr_ctx = NULL;
    int data_size = 0;
    int ret = 0;
    int64_t src_ch_layout = codecCtx->channel_layout;
    int64_t dst_ch_layout = AV_CH_LAYOUT_STEREO;
    int dst_nb_channels = 0;
    int dst_linesize = 0;
    int src_nb_samples = 0;
    int dst_nb_samples = 0;
    int max_dst_nb_samples = 0;
    uint8_t** dst_data = NULL;
    int resampled_data_size = 0;

    swr_ctx = swr_alloc();
    if (!swr_ctx)
    {
        printf("swr_alloc error \n");
        return -1;
    }

    src_ch_layout = (codecCtx->channels ==
                     av_get_channel_layout_nb_channels(codecCtx->channel_layout)) ?
                    codecCtx->channel_layout :
                    av_get_default_channel_layout(codecCtx->channels);

    //这里的设置很粗糙，最好详细处理
    switch (out_channels)
    {
    case 1:
        dst_ch_layout = AV_CH_LAYOUT_MONO;
        break;
    case 2:
        dst_ch_layout = AV_CH_LAYOUT_STEREO;
        break;
    case 3:
        dst_ch_layout = AV_CH_LAYOUT_SURROUND;
        break;
    case 4:
        dst_ch_layout = AV_CH_LAYOUT_QUAD;
        break;
    case 5:
        dst_ch_layout = AV_CH_LAYOUT_5POINT0;
        break;
    case 6:
        dst_ch_layout = AV_CH_LAYOUT_5POINT1;
        break;
    }

    if (src_ch_layout <= 0)
    {
        printf("src_ch_layout error \n");
        return -1;
    }

    src_nb_samples = frame->nb_samples;
    if (src_nb_samples <= 0)
    {
        printf("src_nb_samples error \n");
        return -1;
    }

    av_opt_set_int(swr_ctx, "in_channel_layout", src_ch_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", codecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codecCtx->sample_fmt, 0);

    av_opt_set_int(swr_ctx, "out_channel_layout", dst_ch_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", out_sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", (AVSampleFormat)out_sample_format, 0);

    if ((ret = swr_init(swr_ctx)) < 0)
    {
        printf("Failed to initialize the resampling context\n");
        return -1;
    }

    max_dst_nb_samples = dst_nb_samples = av_rescale_rnd(src_nb_samples, out_sample_rate, codecCtx->sample_rate, AV_ROUND_UP);
    if (max_dst_nb_samples <= 0)
    {
        printf("av_rescale_rnd error \n");
        return -1;
    }

    dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
    ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, (AVSampleFormat)out_sample_format, 0);
    if (ret < 0)
    {
        printf("av_samples_alloc_array_and_samples error \n");
        return -1;
    }

    dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, codecCtx->sample_rate) + src_nb_samples, out_sample_rate, codecCtx->sample_rate, AV_ROUND_UP);
    if (dst_nb_samples <= 0)
    {
        printf("av_rescale_rnd error \n");
        return -1;
    }
    if (dst_nb_samples > max_dst_nb_samples)
    {
        av_free(&dst_data[0]);
        ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, (AVSampleFormat)out_sample_format, 1);
        max_dst_nb_samples = dst_nb_samples;
    }

    if (swr_ctx)
    {
        ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
        if (ret < 0)
        {
            printf("swr_convert error \n");
            return -1;
        }

        resampled_data_size = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels, ret, (AVSampleFormat)out_sample_format, 1);
        if (resampled_data_size < 0)
        {
            printf("av_samples_get_buffer_size error \n");
            return -1;
        }
    }
    else
    {
        printf("swr_ctx null error \n");
        return -1;
    }

    memcpy(out_buf, dst_data[0], resampled_data_size);
    swr_close(swr_ctx);

    if (dst_data)
    {
        av_freep(&dst_data[0]);
    }
    av_freep(&dst_data);
    dst_data = NULL;

    if (swr_ctx)
    {
        swr_free(&swr_ctx);
    }
    //printf("%d\n", resampled_data_size);
    return resampled_data_size;

}

