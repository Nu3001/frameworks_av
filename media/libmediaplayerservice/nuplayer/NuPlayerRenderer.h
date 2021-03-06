/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NUPLAYER_RENDERER_H_

#define NUPLAYER_RENDERER_H_

#include "NuPlayer.h"

namespace android {

struct ABuffer;

struct NuPlayer::Renderer : public AHandler {
    enum Flags {
        FLAG_REAL_TIME = 1,
    };
    Renderer(const sp<MediaPlayerBase::AudioSink> &sink,
             const sp<AMessage> &notify,
             uint32_t flags = 0);

    void queueBuffer(
            bool audio,
            const sp<ABuffer> &buffer,
            const sp<AMessage> &notifyConsumed);
	
	void rendervideobuffer(
		bool audio,
		const sp<ABuffer> &buffer,
		const sp<AMessage> &notifyConsumed) ;

    void queueEOS(bool audio, status_t finalResult);

    void flush(bool audio);

    void signalTimeDiscontinuity();

    void signalAudioSinkChanged();

    void pause();
    void resume();
	void set_wifidisplay_flag(int flag){wifidisplay_flag = flag;};
	int Wifidisplay_set_TimeInfo(int64_t start_time,int64_t audio_start_time);

    enum {
        kWhatEOS                 = 'eos ',
        kWhatFlushComplete       = 'fluC',
        kWhatPosition            = 'posi',
        kWhatVideoRenderingStart = 'vdrd',
        kWhatMediaRenderingStart = 'mdrd',
    };

protected:
    virtual ~Renderer();

    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    enum {
        kWhatDrainAudioQueue    = 'draA',
        kWhatDrainVideoQueue    = 'draV',
        kWhatQueueBuffer        = 'queB',
        kWhatQueueEOS           = 'qEOS',
        kWhatFlush              = 'flus',
        kWhatAudioSinkChanged   = 'auSC',
        kWhatPause              = 'paus',
        kWhatResume             = 'resm',
    };

    struct QueueEntry {
        sp<ABuffer> mBuffer;
        sp<AMessage> mNotifyConsumed;
        size_t mOffset;
        status_t mFinalResult;
    };

    static const int64_t kMinPositionUpdateDelayUs;

    sp<MediaPlayerBase::AudioSink> mAudioSink;
    sp<AMessage> mNotify;
    uint32_t mFlags;
    List<QueueEntry> mAudioQueue;
    List<QueueEntry> mVideoQueue;
    uint32_t mNumFramesWritten;

    bool mDrainAudioQueuePending;
    bool mDrainVideoQueuePending;
    int32_t mAudioQueueGeneration;
    int32_t mVideoQueueGeneration;

    int64_t mAnchorTimeMediaUs;
    int64_t mAnchorTimeRealUs;

    Mutex mFlushLock;  // protects the following 2 member vars.
    bool mFlushingAudio;
    bool mFlushingVideo;

    bool mHasAudio;
    bool mHasVideo;
    bool mSyncQueues;

    bool mPaused;
    bool mVideoRenderingStarted;
    int32_t mVideoRenderingStartGeneration;
    int32_t mAudioRenderingStartGeneration;

    int64_t mLastPositionUpdateUs;
    int64_t mVideoLateByUs;
	int64_t	audio_latency_time;
    int		wifidisplay_flag;
	int64_t sys_start_time ;
	int64_t audio_start_timeUs;
	int64_t last_adujst_time;
	int64_t last_timeUs; 

    bool onDrainAudioQueue();
    void postDrainAudioQueue(int64_t delayUs = 0);

    void onDrainVideoQueue();
    void postDrainVideoQueue();

    void prepareForMediaRenderingStart();
    void notifyIfMediaRenderingStarted();

    void onQueueBuffer(const sp<AMessage> &msg);
    void onQueueEOS(const sp<AMessage> &msg);
    void onFlush(const sp<AMessage> &msg);
    void onAudioSinkChanged();
    void onPause();
    void onResume();

    void notifyEOS(bool audio, status_t finalResult);
    void notifyFlushComplete(bool audio);
    void notifyPosition();
    void notifyVideoLateBy(int64_t lateByUs);
    void notifyVideoRenderingStart();

    void flushQueue(List<QueueEntry> *queue);
    bool dropBufferWhileFlushing(bool audio, const sp<AMessage> &msg);
    void syncQueuesDone();

    DISALLOW_EVIL_CONSTRUCTORS(Renderer);
};

}  // namespace android

#endif  // NUPLAYER_RENDERER_H_
