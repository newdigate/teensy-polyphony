/* Audio Library for Teensy
 * Copyright (c) 2021, Nic Newdigate
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TEENSY_AUDIO_SAMPLER_H
#define TEENSY_AUDIO_SAMPLER_H

#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include "polyphonicsampler.h"
#include <vector>
#include "effect_envelope.h"
#include "mixer.h"
#include "loopsamplerenums.h"
//template <unsigned MAX_NUM_POLYPHONY>
class audiosample {
public:
    audiosample(uint8_t noteNumber, uint8_t noteChannel) : 
        _noteNumber(noteNumber), 
        _noteChannel(noteChannel),
        _data(nullptr), 
        _sampleLength(0), 
        _numChannels(0),
        _filename(nullptr),
        _isSdFile(false),
        _isWavFile(false)
    {

    }
    audiosample(uint8_t noteNumber, uint8_t noteChannel, int16_t *data, uint32_t sampleLength, uint16_t numChannels) : 
        _noteNumber(noteNumber), 
        _noteChannel(noteChannel),
        _data(data), 
        _sampleLength(sampleLength), 
        _numChannels(numChannels) {
    }

    audiosample(uint8_t noteNumber, uint8_t noteChannel, const char *filename, uint16_t numChannels) : 
        _noteNumber(noteNumber), 
        _noteChannel(noteChannel),
        _data(nullptr), 
        _sampleLength(0), 
        _numChannels(numChannels),
        _filename(filename),
        _isSdFile(true),
        _isWavFile(false) {
    }

    audiosample(uint8_t noteNumber, uint8_t noteChannel, const char *filename) : 
        _noteNumber(noteNumber), 
        _noteChannel(noteChannel),
        _data(nullptr), 
        _sampleLength(0), 
        _numChannels(0),
        _filename(filename),
        _isSdFile(true),
        _isWavFile(true) {
    }

    uint8_t _noteNumber;   
    uint8_t _noteChannel;    

    int16_t *_data; 
    uint32_t _sampleLength;
    uint16_t _numChannels;

    bool _isSdFile = false; // is this sample a file or an array
    bool _isWavFile = false; // is the file wav or raw format
    const char *_filename;
private:
};

template <class TAudioPlay>
class audiovoice {
public:
    audiovoice(TAudioPlay *audioplayarray, AudioEffectEnvelope *audioenvelop,  AudioEffectEnvelope *audioenvelop2, AudioMixer4 *audiomixer, AudioMixer4 *audiomixer2, uint8_t mixerChannel, uint8_t mixerChannel2) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(audioenvelop),
        _audioenvelop2(audioenvelop2),
        _audiomixer(audiomixer),
        _audiomixer2(audiomixer2),
        _mixerChannel(mixerChannel),
        _mixerChannel2(mixerChannel2),
        _isStereo(true) {
    }

    audiovoice(TAudioPlay *audioplayarray, AudioEffectEnvelope *audioenvelop, AudioMixer4 *audiomixer, uint8_t mixerChannel) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(audioenvelop),
        _audioenvelop2(nullptr),
        _audiomixer(audiomixer),
        _audiomixer2(nullptr),
        _mixerChannel(mixerChannel),
        _mixerChannel2(0)
    {
    }

    audiovoice(TAudioPlay *audioplayarray, AudioEffectEnvelope *audioenvelop) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(audioenvelop),
        _audioenvelop2(nullptr),
        _audiomixer(nullptr),
        _audiomixer2(nullptr),
        _mixerChannel(0),
        _mixerChannel2(0)
    {
    }

    audiovoice(TAudioPlay *audioplayarray, AudioEffectEnvelope *audioenvelop1, AudioEffectEnvelope *audioenvelop2) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(audioenvelop1),
        _audioenvelop2(audioenvelop2),
        _audiomixer(nullptr),
        _audiomixer2(nullptr),
        _mixerChannel(0),
        _mixerChannel2(0)
    {
    }

    audiovoice(TAudioPlay *audioplayarray) : 
        _audioplayarray(audioplayarray),
        _audioenvelop(nullptr),
        _audioenvelop2(nullptr),
        _audiomixer(nullptr),
        _audiomixer2(nullptr),
        _mixerChannel(0),
        _mixerChannel2(0) {
    }

    audiovoice(TAudioPlay *audioplayarray, AudioMixer4 *audiomixer, uint8_t mixerChannel) :
        _audioplayarray(audioplayarray), 
        _audiomixer(audiomixer),
        _mixerChannel(mixerChannel) {
    }
    audiovoice(TAudioPlay *audioplayarray, AudioMixer4 *audiomixer, uint8_t mixerChannel, AudioMixer4 *audiomixer2, uint8_t mixerChannel2) :
        _audioplayarray(audioplayarray), 
        _audiomixer(audiomixer),
        _audiomixer2(audiomixer2),
        _mixerChannel(mixerChannel),
        _mixerChannel2(mixerChannel2) {
    }
    
    audiovoice(const audiovoice&) = delete;
    virtual ~audiovoice() {
    }

    TAudioPlay *_audioplayarray = nullptr;

    AudioEffectEnvelope *_audioenvelop = nullptr;
    AudioEffectEnvelope *_audioenvelop2 = nullptr;
    AudioMixer4 *_audiomixer = nullptr;
    AudioMixer4 *_audiomixer2 = nullptr;

    uint8_t _mixerChannel;
    uint8_t _mixerChannel2;
    bool _isStereo = false;
    bool isPlaying() {
        return _audioplayarray->isPlaying();
    }
};

template <typename TSample>
class samplermodel {
    public:
        samplermodel() : 
            _channelNotes()        
        {
        }
        
        samplermodel(const samplermodel&) = delete;

        virtual ~samplermodel() {
            for (auto && channelNoteMap : _channelNotes) {
                for (auto &&  noteIndex : *(channelNoteMap.second))
                    delete noteIndex.second;
                delete channelNoteMap.second;
            }
        }

        virtual triggertype getTriggerTypeForChannelAndKey(uint8_t channel, uint8_t note) {
            return triggertype::triggertype_play_while_notedown;
        }

        TSample* getNoteForChannelAndKey(uint8_t channel, uint8_t note) {
            if (_channelNotes.find(channel) == _channelNotes.end() ) {
                return nullptr;
            }
            std::map<uint8_t, TSample*>* channelNotes = _channelNotes[channel];
            if (channelNotes == nullptr)
                return nullptr;

            if (channelNotes->find(note) == channelNotes->end()){
                return nullptr;
            }

            return (*channelNotes)[note];
        }

        TSample* allocateNote(uint8_t channel, uint8_t note, TSample *samplerNote) {
            if (getNoteForChannelAndKey(channel, note) != nullptr){
                // note is already allocated
                return nullptr;
            }
            std::map<uint8_t, TSample*>* channelNotes = nullptr;
            if (_channelNotes.find(channel) == _channelNotes.end() ) {
                channelNotes = new std::map<uint8_t, TSample*>();
                _channelNotes[channel] = channelNotes;
            } else
                channelNotes = _channelNotes[channel];

            (*channelNotes)[note] = samplerNote;
            return samplerNote;
        }

        TSample* findNearestSampleForNoteAndChannel(uint8_t noteNumber, uint channel) {
            std::map<uint8_t, TSample*>* channelNotes = _channelNotes[channel];
            if (channelNotes == nullptr)
                return nullptr;

            uint8_t smallestDiff = 255;
            TSample *candidate = nullptr;

            for (auto &&channelNote : *channelNotes) {
                uint8_t diff = abs(channelNote.first - noteNumber);
                if (diff < smallestDiff) {
                    smallestDiff = diff;
                    candidate = channelNote.second;
                }
            }
            return candidate;
        }

    private:
        std::map<uint8_t, std::map<uint8_t, TSample*>*> _channelNotes;
};

template<typename TVoice, typename TSample>
class audiosampler : public polyphonicsampler<audiovoice<TVoice>, TSample>   {
public:
    audiosampler(polyphonic<audiovoice<TVoice>> &polyphonic) :  
        polyphonicsampler<audiovoice<TVoice>, TSample>(polyphonic),
        _polyphonic(polyphonic),
        _triggertype(triggertype::triggertype_play_while_notedown)
    {
    }

    audiosampler(
        polyphonic<audiovoice<TVoice>> &polyphonic,
        triggertype                     staticTriggerType
    ) :  
        polyphonicsampler<audiovoice<TVoice>, TSample>(polyphonic, staticTriggerType),
        _polyphonic(polyphonic),
        _triggertype(staticTriggerType)
    {
    }

    audiosampler(const audiosampler&) = delete;
    virtual ~audiosampler() {}

    virtual bool noteDownEventCallback(audiovoice<TVoice> *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) override {
        if (voice == nullptr) {
            Serial.printf("audiosampler::noteDownEventCallback note:%d; channel:%d; velocity:%d; retr:%x; <NULL VOICE> \r\n", noteNumber, noteChannel, velocity, retrigger);
            return false;
        }

        Serial.printf("audiosampler::noteDownEventCallback note:%d; channel:%d; velocity:%d; retr:%x; \r\n", noteNumber, noteChannel, velocity, retrigger);
        if (voice->_audiomixer != nullptr) {                        
            voice->_audiomixer->gain( voice->_mixerChannel, velocity / 255.0);
        }
        if (voice->_audiomixer2 != nullptr) {                        
            voice->_audiomixer2->gain( voice->_mixerChannel2, velocity / 255.0);
        }
        if (voice->_audioenvelop != nullptr) {
            voice->_audioenvelop->noteOn();
        }
        if (voice->_audioenvelop2 != nullptr) {
            voice->_audioenvelop2->noteOn();
        }

        return voiceOnEvent(voice->_audioplayarray, sample, noteNumber, noteChannel, velocity, retrigger);
    }

    virtual void noteUpBeginEventCallback(audiovoice<TVoice> *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
        if (voice == nullptr) {
            Serial.printf("noteUpBeginEventCallback note:%d; channel:%d; <NULL VOICE> \r\n", noteNumber, noteChannel);
            return;
        }

        Serial.printf("noteUpBeginEventCallback note:%d; channel:%d; \r\n", noteNumber, noteChannel);

        // Note off event
        if (voice->_audioenvelop != nullptr) {
            voice->_audioenvelop->noteOff();
        }
        
        if (voice->_audioenvelop2 != nullptr) {
            voice->_audioenvelop2->noteOff();
        }
        voiceOffBeginEvent(voice->_audioplayarray, sample, noteNumber, noteChannel);
    }

    virtual void noteUpEndEventCallback(audiovoice<TVoice> *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
        if (voice == nullptr) {
            Serial.printf("noteUpBeginEventCallback note:%d; channel:%d; <NULL VOICE>\r\n", noteNumber, noteChannel);
            return;
        }
        Serial.printf("noteUpBeginEventCallback note:%d; channel:%d; \r\n", noteNumber, noteChannel);
        voiceOffEndEvent(voice->_audioplayarray, sample, noteNumber, noteChannel);
    }
    
    virtual bool isVoiceStillActive(audiovoice<TVoice> *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
        //if (voice->_audioenvelop != nullptr)
        //    return !voice->_audioenvelop->isIdleOrComplete() && voice->_audioplayarray->isPlaying();
        return voice->_audioplayarray->isPlaying();
    }

    virtual bool isVoiceStillNoteDown(audiovoice<TVoice> *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
        return voice->_audioplayarray->isPlaying();
    }

    virtual TSample* findSampleCallback(uint8_t noteNumber, uint8_t noteChannel) override {
        return nullptr;
    }

    virtual triggertype findTriggerType(uint8_t noteNumber, uint8_t noteChannel) override {
        return triggertype::triggertype_play_while_notedown;
    };

    virtual bool voiceOnEvent(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) {
        Serial.printf("audiosampler::voiceOnEvent note:%d; channel:%d; velocity:%d; retr:%x; \r\n", noteNumber, noteChannel, velocity, retrigger);
        return false;
    }

    virtual void voiceOffBeginEvent(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel) {
        Serial.printf("audiosampler::voiceOffBeginEvent note:%d; channel:%d; \r\n", noteNumber, noteChannel);
    }

    virtual void voiceOffEndEvent(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel) {
        Serial.printf("audiosampler::voiceOffEndEvent note:%d; channel:%d; \r\n", noteNumber, noteChannel);
    }

    void trigger(uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool isNoteOn) {
        Serial.printf("audiosampler::trigger note:%d; channel:%d; \r\n", noteNumber, noteChannel);

        polyphonicsampler<audiovoice<TVoice>, TSample>::preprocessNote(noteNumber, noteChannel, isNoteOn, velocity);
    }

 protected:
    polyphonic<audiovoice<TVoice>> &_polyphonic;
    triggertype _triggertype;
};

template <typename TVoice>
class pitchedaudiosampler : public audiosampler<TVoice, audiosample> {
public:
    pitchedaudiosampler(
        samplermodel<audiosample> &samplermodel, 
        polyphonic<audiovoice<TVoice>> &polyphony
    ): 
        audiosampler<TVoice, audiosample>( polyphony, triggertype::triggertype_play_while_notedown ),
        _samplermodel(samplermodel) 
    {
    }
    
    pitchedaudiosampler(const pitchedaudiosampler&) = delete;

    virtual ~pitchedaudiosampler() {
    }

    audiosample* findSampleCallback(uint8_t noteNumber, uint8_t noteChannel) override {
        return _samplermodel.findNearestSampleForNoteAndChannel(noteNumber, noteChannel);
    }

protected:
    samplermodel<audiosample> &_samplermodel;

    static float calcPitchFactor(uint8_t note, uint8_t rootNoteNumber) {
        float result = powf(2.0, (note-rootNoteNumber) / 12.0);
        return result;
    }
};

template <typename TVoice>
class unpitchedaudiosampler : public audiosampler<TVoice, audiosample> {
public:
    unpitchedaudiosampler(
        samplermodel<audiosample> &samplermodel, 
        polyphonic<audiovoice<TVoice>> &polyphony
    ): 
        audiosampler<TVoice, audiosample>(polyphony, triggertype::triggertype_play_while_notedown),
        _samplermodel(samplermodel) {
    }
    
    unpitchedaudiosampler(const unpitchedaudiosampler&) = delete;

    virtual ~unpitchedaudiosampler() {
    }

    audiosample* findSampleCallback(uint8_t noteNumber, uint8_t noteChannel) override {
        return _samplermodel.getNoteForChannelAndKey(noteNumber, noteChannel);
    }

protected:
    samplermodel<audiosample> &_samplermodel;
};

class arraysampler : public pitchedaudiosampler<AudioPlayArrayResmp> {
public:
    arraysampler(
        samplermodel<audiosample> &samplermodel, 
        polyphonic<audiovoice<AudioPlayArrayResmp>> &polyphonic
    ) : 
        pitchedaudiosampler<AudioPlayArrayResmp>(samplermodel, polyphonic)
    {
    }

    arraysampler(const arraysampler&) = delete;

    virtual ~arraysampler() {
    }

    bool voiceOnEvent(AudioPlayArrayResmp *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) override {
        if (voice == nullptr || sample == nullptr)
            return false;

        float factor = calcPitchFactor(noteNumber, sample->_noteNumber);
        voice->setPlaybackRate(factor);
        return voice->playRaw(sample->_data, sample->_sampleLength, sample->_numChannels);
    }

    void voiceOffBeginEvent(AudioPlayArrayResmp *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }

    void voiceOffEndEvent(AudioPlayArrayResmp *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }

protected:

};

class sdsampler : public pitchedaudiosampler<AudioPlaySdResmp> {
public:
    sdsampler(
        samplermodel<audiosample> &samplermodel, 
        polyphonic<audiovoice<AudioPlaySdResmp>> &polyphonic
    ) : 
        pitchedaudiosampler<AudioPlaySdResmp>(samplermodel, polyphonic)
    {
    }

    sdsampler(const sdsampler&) = delete;

    virtual ~sdsampler() {
    }

    bool voiceOnEvent(AudioPlaySdResmp *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) override {
        if (voice == nullptr || sample == nullptr || sample->_filename == nullptr)
            return false;

        float factor = calcPitchFactor(noteNumber, sample->_noteNumber);
        voice->setPlaybackRate(factor);
        return voice->playRaw( sample->_filename, sample->_numChannels);
    }

    void voiceOffBeginEvent(AudioPlaySdResmp *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }

    void voiceOffEndEvent(AudioPlaySdResmp *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }
};

class unpitchedsdwavsampler : public unpitchedaudiosampler<AudioPlaySdWav> {
public:
    unpitchedsdwavsampler (
        samplermodel<audiosample> &samplermodel, 
        polyphonic<audiovoice<AudioPlaySdWav>> &polyphonic
    ) : 
        unpitchedaudiosampler<AudioPlaySdWav>(samplermodel, polyphonic)
    {
    }

    unpitchedsdwavsampler(const unpitchedsdwavsampler&) = delete;

    virtual ~unpitchedsdwavsampler() {
    }

    bool voiceOnEvent(AudioPlaySdWav *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) override {
        if (voice == nullptr || sample == nullptr || sample->_filename == nullptr)
            return false;

        return voice->play(sample->_filename);
    }

    void voiceOffBeginEvent(AudioPlaySdWav *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }

    void voiceOffEndEvent(AudioPlaySdWav *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }

};

class unpitchedsdrawsampler : public unpitchedaudiosampler<AudioPlaySdRaw> {
public:
    unpitchedsdrawsampler (
        samplermodel<audiosample> &samplermodel, 
        polyphonic<audiovoice<AudioPlaySdRaw>> &polyphonic
    ) : 
        unpitchedaudiosampler<AudioPlaySdRaw>(samplermodel, polyphonic)
    {
    }

    unpitchedsdrawsampler(const unpitchedsdrawsampler&) = delete;

    virtual ~unpitchedsdrawsampler() {
    }

    bool voiceOnEvent(AudioPlaySdRaw *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) override {
        if (voice == nullptr || sample == nullptr || sample->_filename == nullptr)
            return false;

        return voice->play(sample->_filename);
    }

    void voiceOffBeginEvent(AudioPlaySdRaw *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }

    void voiceOffEndEvent(AudioPlaySdRaw *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel) override {
    }
};

class unpitchedmemorysampler : public unpitchedaudiosampler<AudioPlayMemory> {
public:
    unpitchedmemorysampler (
        samplermodel<audiosample> &samplermodel, 
        polyphonic<audiovoice<AudioPlayMemory>> &polyphonic
    ) : 
        unpitchedaudiosampler<AudioPlayMemory>(samplermodel, polyphonic)
    {
    }

    unpitchedmemorysampler(const unpitchedmemorysampler&) = delete;

    virtual ~unpitchedmemorysampler() {
    }

    bool voiceOnEvent(AudioPlayMemory *voice, audiosample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) override {

        if (voice == nullptr || sample == nullptr || sample->_filename == nullptr)
            return false;

        voice->play((unsigned int *)sample->_data);// , sample->_sampleLength, sample->_numChannels);
        return true;
    }

};

#endif