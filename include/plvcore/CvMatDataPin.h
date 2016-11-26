#ifndef CVMATDATAOUTPUTPIN_H
#define CVMATDATAOUTPUTPIN_H

#include "IInputPin.h"
#include "IOutputPin.h"
#include "CvMatData.h"

namespace plv
{
    class CvMatDataOutputPin;
    class PipelineElement;
    class DataConsumer;
    class DataProducer;

    class PLVCORE_EXPORT CvMatDataInputPin : public IInputPin
    {
    public:
        CvMatDataInputPin( const QString& name,
                           DataConsumer* owner,
                           Required required = CONNECTION_REQUIRED,
                           Synchronized synchronous = CONNECTION_SYNCHRONOUS,
                           DataGuarantee guarantee = CONNECTION_GUARANTEES_DATA) :
                           IInputPin( name, owner, required, synchronous, guarantee )
        {
        }

        CvMatData get();

        void addSupportedDepth(int depth);
        void addSupportedChannels(int channels);

        void removeSupportedDepth(int depth);
        void removeSupportedChannels(int channels);

        bool supportsChannels( int channels ) const;
        bool supportsDepth( int depth ) const;

        void clearDephts();
        void clearChannels();

        void addAllDepths();
        void addAllChannels();

        /** Done at runtime */
        void checkImageFormat( const CvMatData& img );

        /** Done once at connection creation only */
        virtual bool acceptsConnectionWith(const IOutputPin* pin, QString& errStr) const;

        //virtual const std::type_info& getTypeInfo() const;
        virtual int getTypeId() const;
        virtual QString getTypeName() const;

    private:
        QSet<int> m_depths;
        QSet<int> m_channels;
    };

    class PLVCORE_EXPORT CvMatDataOutputPin : public IOutputPin
    {
    public:
        CvMatDataOutputPin( const QString& name, DataProducer* owner ) :
                IOutputPin( name, owner ) {}

        /** Puts data in connection. Drops data if no connection present. */
        void put( CvMatData img );

        void addSupportedDepth(int depth);
        void addSupportedChannels(int channels);
        void removeSupportedDepth(int depth);
        void removeSupportedChannels(int channels);
        bool supportsChannels( int channels ) const;
        bool supportsDepth( int depth ) const;

        void clearDephts();
        void clearChannels();

        void addAllDepths();
        void addAllChannels();

        /** Done at runtime */
        void checkImageFormat( const CvMatData& img );

        /** Done once at connection creation only */
        virtual bool acceptsConnectionWith(const IInputPin* pin, QString& errStr) const;

        //virtual const std::type_info& getTypeInfo() const;
        virtual int getTypeId() const;
        virtual QString getTypeName() const;

    protected:
        QSet<int> m_depths;
        QSet<int> m_channels;
    };

    PLVCORE_EXPORT CvMatDataOutputPin* createCvMatDataOutputPin( const QString& name, DataProducer* owner )
    throw (IllegalArgumentException);

    PLVCORE_EXPORT CvMatDataInputPin* createCvMatDataInputPin( const QString& name, DataConsumer* owner,
                                  IInputPin::Required required = IInputPin::CONNECTION_REQUIRED,
                                  IInputPin::Synchronized synchronous = IInputPin::CONNECTION_SYNCHRONOUS  )
    throw (IllegalArgumentException);
}

#endif // CvMatDataOUTPUTPIN_H
