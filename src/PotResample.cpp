#include "PotResample.h"
#include "fmt1.h"

PotResample::PotResample()
{
}

PotResample::~PotResample()
{
}

int PotResample::convert(AVCodecContext* codec_ctx, AVFrame* frame, int out_sample_rate, int out_channels, uint8_t* out_buf)
{
    SwrContext* swr_ctx = NULL;
    int data_size = 0;
    int ret = 0;
    AVChannelLayout src_ch_layout = codec_ctx->ch_layout;
    AVChannelLayout dst_ch_layout = src_ch_layout;

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
        fmt1::print("swr_alloc error \n");
        return -1;
    }

    av_channel_layout_default(&src_ch_layout, codec_ctx->ch_layout.nb_channels);

    //这里的设置很粗糙，最好详细处理
    switch (out_channels)
    {
    case 1:
        dst_ch_layout = AV_CHANNEL_LAYOUT_MONO;
        break;
    case 2:
        dst_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
        break;
    case 3:
        dst_ch_layout = AV_CHANNEL_LAYOUT_SURROUND;
        break;
    case 4:
        dst_ch_layout = AV_CHANNEL_LAYOUT_QUAD;
        break;
    case 5:
        dst_ch_layout = AV_CHANNEL_LAYOUT_5POINT0;
        break;
    case 6:
        dst_ch_layout = AV_CHANNEL_LAYOUT_5POINT1;
        break;
    }

    if (src_ch_layout.u.mask == 0)
    {
        fmt1::print("src_ch_layout error \n");
        return -1;
    }

    src_nb_samples = frame->nb_samples;
    if (src_nb_samples <= 0)
    {
        fmt1::print("src_nb_samples error \n");
        return -1;
    }

    av_opt_set_chlayout(swr_ctx, "in_chlayout", &src_ch_layout, 0);
    av_opt_set_chlayout(swr_ctx, "out_chlayout", &dst_ch_layout, 0);

    av_opt_set_int(swr_ctx, "in_sample_rate", codec_ctx->sample_rate, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", out_sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codec_ctx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", (AVSampleFormat)out_sample_format_, 0);

    if ((ret = swr_init(swr_ctx)) < 0)
    {
        fmt1::print("Failed to initialize the resampling context\n");
        return -1;
    }

    max_dst_nb_samples = dst_nb_samples = av_rescale_rnd(src_nb_samples, out_sample_rate, codec_ctx->sample_rate, AV_ROUND_UP);
    if (max_dst_nb_samples <= 0)
    {
        fmt1::print("av_rescale_rnd error \n");
        return -1;
    }

    dst_nb_channels = dst_ch_layout.nb_channels;
    ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, (AVSampleFormat)out_sample_format_, 0);
    if (ret < 0)
    {
        fmt1::print("av_samples_alloc_array_and_samples error \n");
        return -1;
    }

    dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, codec_ctx->sample_rate) + src_nb_samples, out_sample_rate, codec_ctx->sample_rate, AV_ROUND_UP);
    if (dst_nb_samples <= 0)
    {
        fmt1::print("av_rescale_rnd error \n");
        return -1;
    }
    if (dst_nb_samples > max_dst_nb_samples)
    {
        av_free(&dst_data[0]);
        ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels, dst_nb_samples, (AVSampleFormat)out_sample_format_, 1);
        max_dst_nb_samples = dst_nb_samples;
    }

    if (swr_ctx)
    {
        ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
        if (ret < 0)
        {
            fmt1::print("swr_convert error \n");
            return -1;
        }

        resampled_data_size = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels, ret, (AVSampleFormat)out_sample_format_, 1);
        if (resampled_data_size < 0)
        {
            fmt1::print("av_samples_get_buffer_size error \n");
            return -1;
        }
    }
    else
    {
        fmt1::print("swr_ctx null error \n");
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
    //fmt1::print("%d\n", resampled_data_size);
    return resampled_data_size;
}
