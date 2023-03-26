#ifndef TEENSY_MYSAMPLER_H
#define TEENSY_MYSAMPLER_H

#include "sdsampleplayernote.h"
#include "polyphonic.h"
#include <TeensyVariablePlayback.h>
#include <tuple>
#include <sampler.h>
#include "loopsampler.h"
#include "audiovoicepolyphonic.h"

namespace newdigate
{
    typedef std::function<void(unsigned index, unsigned sampleNumber)> CallbackFn;

    class ProgressRegistration {
        public:
            ProgressRegistration(unsigned int id, unsigned int resolution, CallbackFn callback, char *filename, uint8_t noteNumber, uint8_t noteChannel) :
                _id(id),
                _resolution(resolution),
                _callback(callback),
                _filename(filename),
                _noteNumber(noteNumber),
                _noteChannel(noteChannel)
            {
            }

            ProgressRegistration(const ProgressRegistration& progressRegistration) = delete;

            virtual ~ProgressRegistration() { }

            unsigned int _id;
            unsigned int _resolution;
            CallbackFn _callback;
            char *_filename;
            uint8_t _noteNumber;
            uint8_t _noteChannel;
    };

    class ProgressIndictation {
    public:
        ProgressIndictation(
            AudioPlaySdResmp &voice,
            unsigned int id, 
            ProgressRegistration &registration
        ) : 
            _voice(voice),
            Id(id), 
            _lastValue(0), 
            _registration(registration) {
        }
        
        ProgressIndictation(const ProgressIndictation &indicator)  = delete;

        virtual ~ProgressIndictation() {
        }

        unsigned int Id;
        AudioPlaySdResmp &_voice;
        ProgressRegistration &_registration;
        unsigned int _lastValue;

    protected:
    };

    class MyLoopSampler : public audiosampler<AudioPlaySdResmp, sdsampleplayernote> {
    public:
        MyLoopSampler(
            samplermodel<sdsampleplayernote> &samplermodel, 
            audiovoicepolyphonic<AudioPlaySdResmp> &polyphony
        ): 
            audiosampler<AudioPlaySdResmp, sdsampleplayernote>(
                polyphony, 
                [&] (uint8_t noteNumber, uint8_t noteChannel) -> triggertype {
                    sdsampleplayernote *sample = _samplermodel.getNoteForChannelAndKey(noteChannel, noteNumber);
                    if (sample == nullptr)
                        return triggertype::triggertype_play_while_notedown;
                    return sample->_triggertype;
                }),
            _samplermodel(samplermodel),
            _currentId(0),
            _currentIndicatorId(0) 
        {
        }
        
        MyLoopSampler(const MyLoopSampler&) = delete;

        virtual ~MyLoopSampler() {
        }

        void voiceOnEvent(AudioPlaySdResmp *voice, sdsampleplayernote *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity) override {
            Serial.println("Voice ON event");
            if (sample == nullptr || sample->_filename == nullptr)
                return;

            AudioPlaySdResmp &audioplay = *voice;
            switch (sample->_playdirection) {
                case playdirection_begin_forward: audioplay.setPlaybackRate(1.0); break; 
                case playdirection_begin_backward: audioplay.setPlaybackRate(-1.0); break;
            }

            switch (sample->_playlooptype) {
                case playlooptype_once: audioplay.setLoopType(loop_type::looptype_none); break;
                case playlooptype_looping: audioplay.setLoopType(loop_type::looptype_repeat); break;
                case playlooptype_pingpong: audioplay.setLoopType(loop_type::looptype_pingpong); break;
            }

            audioplay.playWav(sample->_filename);
            createIndicatorForRegistrations(noteNumber, noteChannel, sample->_filename, voice);

            //sample->_voice = voice;
        }

        void voiceOffEvent(AudioPlaySdResmp *voice, sdsampleplayernote *sample, uint8_t noteNumber, uint8_t noteChannel) override {
            Serial.printf("Voice OFF event: note: %d; channel: %d\r\n", noteNumber, noteChannel);

            std::map<uint8_t, std::vector< ProgressRegistration*>*> *progressRegistrationsForChannel = _progressRegistrations[noteChannel];
            if (progressRegistrationsForChannel == nullptr) {
                Serial.printf("\tNo Progress Registrations for channel: %d\r\n", noteChannel);
                return;
            }

            std::vector<ProgressRegistration*> *progressRegsForNoteAndChannel = (*progressRegistrationsForChannel)[noteNumber];
            if (progressRegsForNoteAndChannel == nullptr) {
                Serial.printf("\tNo progress reg for: note: %d; channel: %d\r\n", noteNumber, noteChannel);
                return;
            }
            for (auto && progReg : *progressRegsForNoteAndChannel) {
                if (progReg == nullptr)
                    continue;

                std::vector< ProgressIndictation* > *progressIndicators = _progressIndicatorsByRegId[progReg->_id];

                if (progressIndicators != nullptr) {
                    if (progressIndicators->size() > 0)
                        progressIndicators->erase( 
                            std::remove_if(
                                std::begin( *progressIndicators ),
                                std::end( *progressIndicators ),
                                [noteNumber, noteChannel, voice] (const ProgressIndictation *ind) -> bool { 
                                    return 
                                        ind->_registration._noteNumber == noteNumber 
                                        && ind->_registration._noteChannel == noteChannel
                                        && &(ind->_voice) == voice; 
                                } 
                            ), std::end (*progressIndicators) );
                }
                /*
                for (auto && ind : *(reg.second)) {


                    std::vector<ProgressIndictation*>::iterator it = std::find_if( reg.second->begin(), reg.second->end(), 
                        [noteNumber, noteChannel] (const ProgressIndictation *ind) -> bool { 
                            return ind->_registration._noteNumber == noteNumber && ind->_registration._noteChannel == noteChannel; 
                        } );
                    if (it != reg.second->end())
                        reg.second->erase(it);
                }
                */
            }

            //_progressIndicatorsByRegId.remove();
        }

        void voiceRetriggerEvent(AudioPlaySdResmp *voice, sdsampleplayernote *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity) override {

        }

        sdsampleplayernote* findSample(uint8_t noteNumber, uint8_t noteChannel) override {
            return _samplermodel.getNoteForChannelAndKey(noteChannel, noteNumber);
        }

        unsigned int registerProgressCallback(uint8_t noteNumber, uint8_t noteChannel, char *filename, unsigned int resolution, CallbackFn callback) {
            Serial.printf("registerProgressCallback: note: %d; channel: %d; filename: %s; res: %d \r\n", noteNumber, noteChannel, filename, resolution);
            std::map<uint8_t, std::vector< ProgressRegistration*>*> *progressRegistrationsForChannel = _progressRegistrations[noteChannel];
            if (progressRegistrationsForChannel == nullptr) {
                progressRegistrationsForChannel = new std::map<uint8_t, std::vector< ProgressRegistration*>*>();
                _progressRegistrations[noteChannel] = progressRegistrationsForChannel;
            }

            std::vector<ProgressRegistration*> *progressRegsForNoteAndChannel = (*progressRegistrationsForChannel)[noteNumber];
            if (progressRegsForNoteAndChannel == nullptr) {
                progressRegsForNoteAndChannel = new std::vector<ProgressRegistration*>();
                (*progressRegistrationsForChannel)[noteNumber] = progressRegsForNoteAndChannel;
            }

            ProgressRegistration *progReg = new ProgressRegistration(_currentId++, resolution, callback, filename, noteNumber, noteChannel);
            progressRegsForNoteAndChannel->push_back(progReg);
            _progressRegistrationsById[progReg->_id] = progReg;
            return progReg->_id;
        }

        void unregisterProgressCallback(unsigned int registrationId){
            Serial.printf("unregisterProgressCallback: %d; channel: %d; filename: %s; res: %d \r\n", registrationId);

            ProgressRegistration *progReg = _progressRegistrationsById[registrationId];
            if (progReg == nullptr) {
                Serial.printf("\tNOT FOUND: unregisterProgressCallback: %d; channel: %d; filename: %s; res: %d \r\n", registrationId);
                return;
            }

            uint8_t noteNumber = progReg->_noteNumber;
            uint8_t noteChannel = progReg->_noteChannel;

            _progressRegistrationsById.erase(registrationId);

            std::map<uint8_t, std::vector< ProgressRegistration*>*> *progressRegistrationsForChannel = _progressRegistrations[noteChannel];
            if (progressRegistrationsForChannel == nullptr) {
                delete progReg;
                return;
            }

            std::vector<ProgressRegistration*> *progressRegsForNoteAndChannel = (*progressRegistrationsForChannel)[noteNumber];
            if (progressRegsForNoteAndChannel == nullptr) {
                delete progReg;
                return;
            }
            /*
            std::vector<ProgressRegistration*>::iterator position = 
                std::find_if(
                    progressRegsForNoteAndChannel->begin(), 
                    progressRegsForNoteAndChannel->end(), 
                    [registrationId] 
                        (ProgressRegistration* pr) -> bool { 
                            return pr->_id == registrationId; 
                        });

            if (position != progressRegsForNoteAndChannel->end())
                progressRegsForNoteAndChannel->erase(position); */
            progressRegsForNoteAndChannel->erase(
                std::remove_if( 
                    std::begin(*progressRegsForNoteAndChannel), 
                    std::end(*progressRegsForNoteAndChannel), 
                    [registrationId] (ProgressRegistration* pr) -> bool { 
                        return pr->_id == registrationId; 
                    }));
            std::vector<ProgressIndictation*> *progressIndicationsForRegId = _progressIndicatorsByRegId[registrationId];
            if (progressIndicationsForRegId != nullptr) {
                for (auto && reg : *progressIndicationsForRegId){
                    delete reg;
                }
                _progressIndicatorsByRegId.erase(registrationId);
                delete progressIndicationsForRegId;
            }
            delete progReg;
        }

        void createIndicatorForRegistrations(uint8_t noteNumber, uint8_t noteChannel, char *filename, AudioPlaySdResmp *voice) {
            Serial.printf("createIndicatorForRegistrations: note: %d; channel: %d; filename: %s; \r\n", noteNumber, noteChannel, filename);
            std::map<uint8_t, std::vector<ProgressRegistration*>*>* progressRegistrationsForChannel = _progressRegistrations[noteChannel];
            if (progressRegistrationsForChannel == nullptr)
                return;

            std::vector<ProgressRegistration*> *progressRegsForNoteAndChannel = (*progressRegistrationsForChannel)[noteNumber];
            if (progressRegsForNoteAndChannel == nullptr) {
                return;
            }

            for (auto && reg : *progressRegsForNoteAndChannel) {
                if (reg->_filename == filename) {
                    ProgressIndictation *indicator = new ProgressIndictation(*voice, _currentIndicatorId++, *reg);
                    std::vector<ProgressIndictation*> *progressIndicatorsForRegId = _progressIndicatorsByRegId[reg->_id];

                    if (progressIndicatorsForRegId == nullptr) {
                        progressIndicatorsForRegId = new std::vector<ProgressIndictation*>();
                        _progressIndicatorsByRegId[reg->_id] = progressIndicatorsForRegId;

                    }
                    progressIndicatorsForRegId->push_back(indicator);
                }
            }
        }

        void updateProgress() {
            for (auto && reg : _progressIndicatorsByRegId) {
                if (reg.second != nullptr)
                    for (auto && ind : *(reg.second)) {
                        updateProgressRegistration(ind);
                    }
            }
        }

        void update() {
             audiosampler<AudioPlaySdResmp, sdsampleplayernote>::update(); 
        }

        void updateProgressRegistration(ProgressIndictation *reg) {
            if  (reg->_voice.isPlaying() ) {
                int bufferPosition = reg->_voice.getBufferPosition1();
                int progress = (bufferPosition * reg->_registration._resolution) / reg->_voice.getLoopFinish();
                if (progress != reg->_lastValue) {
                    // call callback
                    reg->_registration._callback(reg->Id, progress);
                    reg->_lastValue = progress;
                }
            };
        }


    protected:
        samplermodel<sdsampleplayernote> &_samplermodel;
        unsigned int _currentId, _currentIndicatorId;
        std::map<uint8_t, std::map<uint8_t, std::vector<ProgressRegistration*>*>*> _progressRegistrations;
        std::map<unsigned int, ProgressRegistration*> _progressRegistrationsById;
        std::map<unsigned int, std::vector<ProgressIndictation*>*> _progressIndicatorsByRegId;

        static float calcPitchFactor(uint8_t note, uint8_t rootNoteNumber) {
            float result = powf(2.0, (note-rootNoteNumber) / 12.0);
            return result;
        }
    };

} // namespace newdigate

#endif
