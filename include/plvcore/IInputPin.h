#ifndef IINPUTPIN_H
#define IINPUTPIN_H

#include "Pin.h"
#include "PinConnection.h"

namespace plv
{
    class DataConsumer;

    class PLVCORE_EXPORT IInputPin : public Pin
    {
        Q_OBJECT

    public:
        typedef enum Required {
            CONNECTION_OPTIONAL,
            CONNECTION_REQUIRED
        } Required;

        typedef enum Synchronized {
            CONNECTION_SYNCHRONOUS,
            CONNECTION_ASYNCHRONOUS
        } Synchronized;

        typedef enum DataGuarantee {
            CONNECTION_GUARANTEES_DATA,
            CONNECTION_DATA_MAY_BE_NULL
        } DataGuarantee;

        IInputPin( const QString& name, DataConsumer* consumer, Required required, Synchronized sync, DataGuarantee guarantee );
        virtual ~IInputPin();

        inline Required getRequiredType() const { return m_required; }
        inline Synchronized getSynchronizedType() const { return m_synchronous; }

        inline bool isRequired() const { return m_required == CONNECTION_REQUIRED; }
        inline bool isOptional() const { return m_required == CONNECTION_OPTIONAL; }

        inline bool isSynchronous() const { return m_synchronous == CONNECTION_SYNCHRONOUS; }
        inline bool isAsynchronous() const { return m_synchronous == CONNECTION_ASYNCHRONOUS; }

        inline bool isDataGuaranteed() const { return m_guarantee == CONNECTION_GUARANTEES_DATA; }
        inline bool canDataBeNull() const { return m_guarantee == CONNECTION_DATA_MAY_BE_NULL; }

        void setConnection(PinConnection* connection);
        void removeConnection();
        PinConnection* getConnection() const;

        void peekNext(unsigned int& serial, bool& isNull) const;

        /** @returns true if there are data items waiting in the queue of the connection this pin has,
         * false if there are no data items or there is no connection
         */
        bool hasDataItems() const;

        /** Same as hasDataItems except it also checks if the first data item in the queue is not a NULL item */
        bool hasData() const;

        void flushConnection();
        bool fastforward( unsigned int target );

        /** @returns true when pin is connected */
        bool isConnected() const;

        /** this method is called before each call to process() in the owner */
        void pre();

        /** this method is called after each call to process() in the owner */
        void post();

        /** removes first data item on the connection. There needs to be a connection. */
        void removeFirst();

        void acceptData( const Data& data );

        /** removes the first data item. Call to clear unwanted data items such as null items on pins without data guarantee */
        void clear();

        /** converts the Data item to a QVariant */
        void getVariant( QVariant& data );

        /** set called to value of val */
        inline void setCalled( bool val ) { m_called = val; }

        /** returns true when get() has been called */
        inline bool isCalled() const { return m_called; }

        /** checks if this pin is compatible with output pin */
        virtual bool acceptsConnectionWith( const IOutputPin* pin, QString& errStr ) const = 0;

        /** methods not implemented from abstract Pin class */
        virtual int getTypeId() const = 0;
        virtual QString getTypeName() const = 0;
        virtual bool isDynamicallyTyped() const { return false; }

    protected:
        DataConsumer* m_consumer;

        /** The input pin required type either CONNECTION_OPTIONAL or CONNECTION_REQUIRED */
        Required m_required;

        /** The input pin synchronicity either CONNECTION_SYNCHRONOUS or CONNECTION_ASYNCHRONOUS*/
        Synchronized m_synchronous;

        /**
         * The input pin guarantees data when pin is CONNECTION_SYNCHRONOUS and guarantee CONNECTION_GUARANTEES_DATA
         * Default is CONNECTION_GUARANTEES_DATA. When using CONNECTION_DATA_CAN_BE_NULL the processor must itself check
         * if there is a NULL data item when taking data from the pin.
        */
        DataGuarantee m_guarantee;

        /** isNull() if there is no connection */
        RefPtr<PinConnection> m_connection;

        /** true when get() has been called */
        bool m_called;
    };
}

#endif

