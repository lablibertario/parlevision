/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "PipelineLoader.h"
#include "PipelineElementFactory.h"
#include "Pipeline.h"
#include "PipelineProcessor.h"
#include "Pin.h"
#include "IInputPin.h"
#include "IOutputPin.h"
#include "Enum.h"

#include <QStringList>
#include <QFile>
#include <QMetaProperty>
#include <QStringBuilder>
#include <QDateTime>
#include <QUrl>

using namespace plv;

PipelineLoader::PipelineLoader()
{
}

PipelineLoader::~PipelineLoader()
{
}

void PipelineLoader::serialize( const QString& filename, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    QFile file( filename );
    if( !file.open(QIODevice::WriteOnly | QIODevice::Text ))
    {
        throw std::runtime_error (
            QString( "Failed to open file " + filename).toStdString() );
    }
    QTextStream out(&file);
    out << serialize( pipeline );
}

QString PipelineLoader::serialize( Pipeline* pl )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    QDomDocument doc;

    QDomElement xmlPipeline = doc.createElement( "pipeline" );
    doc.appendChild( xmlPipeline );

    QDomElement xmlElements = doc.createElement( "elements" );
    xmlPipeline.appendChild( xmlElements );

    const Pipeline::PipelineElementMap& ples = pl->getChildren();
    QMapIterator<int, RefPtr<PipelineElement> > itr( ples );
    while( itr.hasNext() )
    {
        itr.next();
        RefPtr<PipelineElement> ple = itr.value();

        QString className = ple->metaObject()->className();
        int id = ple->getId();

        QDomElement xmlElement = doc.createElement( "element" );
        xmlElement.setAttribute( "id", id );
        xmlElement.setAttribute( "name", className );
        xmlElements.appendChild( xmlElement );

        QDomElement xmlProperties = doc.createElement( "properties" );
        xmlElement.appendChild( xmlProperties );

        // first do static properties
        const QMetaObject* metaObject = ple->metaObject();
        for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
        {
            QMetaProperty property = metaObject->property(i);
            QString propertyName = QString::fromLatin1( property.name() );

            QString propertyValue;
            QVariant::Type propertyType = property.type();

            // custom types are saved here
            if( propertyType == QVariant::UserType )
            {
                QVariant value = property.read( ple );
                if( value.canConvert<plv::Enum>() )
                {
                    plv::Enum e = value.value<plv::Enum>();
                    propertyValue = e.getSelectedItemName();
                }
            }
            // all other QVariant can easily be converted to string
            else
            {
                propertyValue = property.read( ple ).toString();
            }
            QDomElement xmlProperty = doc.createElement( propertyName );
            QDomText text = doc.createTextNode( propertyValue );
            xmlProperty.appendChild( text );
            xmlProperties.appendChild( xmlProperty );
        }

        // now dynamic properties
        // these are not used by processor definitions
        {
            QVariant xVal = ple->property("sceneCoordX");
            QVariant yVal = ple->property("sceneCoordY");
            if( xVal.isValid() && yVal.isValid() )
            {
                QDomElement xmlXValProperty = doc.createElement( "sceneCoordX" );
                QDomText xValText = doc.createTextNode( xVal.toString() );
                xmlXValProperty.appendChild( xValText );
                xmlProperties.appendChild( xmlXValProperty );

                QDomElement xmlYValProperty = doc.createElement( "sceneCoordY" );
                QDomText yValText = doc.createTextNode( yVal.toString() );
                xmlYValProperty.appendChild( yValText );
                xmlProperties.appendChild( xmlYValProperty );
            }
        }
    }

    QDomElement xmlConnections = doc.createElement( "connections" );
    xmlPipeline.appendChild( xmlConnections );

    const Pipeline::PipelineConnectionMap& connections = pl->getConnections();
    foreach( RefPtr<PinConnection> connection, connections )
    {
        QDomElement xmlConnection = doc.createElement( "connection" );
        xmlConnections.appendChild( xmlConnection );

        QDomElement xmlSink = doc.createElement("sink");
        xmlConnection.appendChild( xmlSink );

        QDomElement xmlSinkPinId = doc.createElement( "pinId" );
        QDomElement xmlSinkId = doc.createElement( "processorId" );

        xmlSink.appendChild( xmlSinkPinId );
        xmlSink.appendChild( xmlSinkId );

        QString sinkPinId = QString::number( connection->toPin()->getId() );
        QString sinkId = QString::number( connection->toPin()->getOwner()->getId() );

        QDomText xmlSinkPinIdText = doc.createTextNode( sinkPinId );
        QDomText xmlSinkIdText = doc.createTextNode( sinkId );

        xmlSinkPinId.appendChild( xmlSinkPinIdText );
        xmlSinkId.appendChild( xmlSinkIdText );

        QDomElement xmlSource = doc.createElement("source");
        xmlConnection.appendChild( xmlSource );

        QDomElement xmlSourcePinId = doc.createElement( "pinId" );
        QDomElement xmlSourceId = doc.createElement( "processorId" );

        xmlSource.appendChild( xmlSourcePinId );
        xmlSource.appendChild( xmlSourceId );

        QString sourcePinId = QString::number(connection->fromPin()->getId());
        QString sourceId = QString::number(connection->fromPin()->getOwner()->getId());

        QDomText xmlSourcePinIdText = doc.createTextNode( sourcePinId );
        QDomText xmlSourceIdText = doc.createTextNode( sourceId );

        xmlSourcePinId.appendChild( xmlSourcePinIdText );
        xmlSourceId.appendChild( xmlSourceIdText );
    }
    return doc.toString();
}


void PipelineLoader::deserialize( const QString& filename, Pipeline* pipeline )
        throw(std::runtime_error) /*TODO checked exceptions*/
{
    if( !QFile::exists( filename ) )
    {
        throw std::runtime_error (
            QString( "Failed to open file " + filename + ". File does not exist.").toStdString() );
    }

    QFile file( filename );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw std::runtime_error (
            QString( "Failed to open file " + filename).toStdString() );
    }

    QDomDocument doc;
    doc.setContent( &file );
    try
    {
        deserialize( &doc, pipeline );
    }
    catch( ... )
    {
        file.close();
        throw;
    }
    file.close();
}

void PipelineLoader::deserialize( QDomDocument* document, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    qDebug() << document->elementsByTagName("pipeline").count() << " pipeline, "
             << document->elementsByTagName("element").count() << " elements and "
             << document->elementsByTagName("connection").count() << " connections.";

    QDomNodeList elementsList = document->elementsByTagName( "element" );
    parseElements( &elementsList, pipeline );

    QDomNodeList connectionsList = document->elementsByTagName( "connection" );
    parseConnections( &connectionsList, pipeline );
}

void PipelineLoader::parseElements( QDomNodeList* list, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    for( unsigned int i=0; i < list->length(); ++i )
    {
        QDomNode elementNode = list->item( i );
        QDomNamedNodeMap attributes = elementNode.attributes();

        QDomNode idNode = attributes.namedItem( "id" );
        QString nodeValue = idNode.nodeValue();
        int id = nodeValue.toInt();

        QDomNode nameNode = attributes.namedItem( "name" );
        QString name = nameNode.nodeValue();

        qDebug() << "Creating element with name: " << name << " and id: " << id;

        int typeId = PipelineElementFactory::elementId( name );
        if( typeId == -1 )
        {
            QString msg = "XML document contains unknown processor type " % name;
            throw std::runtime_error ( msg.toStdString() );
        }
        // and instantiate it
        PipelineElement* ple = PipelineElementFactory::construct( typeId );

        // set the id
        ple->setId( id );

        // check if it is valid
        if( !pipeline->canAddElement( ple ) )
        {
            QString msg = "XML parsing failed. PipelineElement with name "
                          % name % " has invalid or used id " + id;
            throw std::runtime_error( msg.toStdString() );
        }
        pipeline->addElement( ple );

        // now parse the properties
        QDomElement propertiesNode = elementNode.firstChildElement( "properties" );
        if( propertiesNode.hasChildNodes() )
        {
            QDomNodeList children = propertiesNode.childNodes();
            for( unsigned int i=0; i < children.length(); ++i )
            {
                QDomNode child = children.item( i );
                QDomElement element = child.toElement();

                if( element.isNull() )
                {
                    //error
                }

                QString propNameXml = element.nodeName();
                QString propValueXml = element.text();

                // convert the data to a known QVariant datatype
                // if the property is unknown it will add the property
                // as string

                const QMetaObject* metaObject = ple->metaObject();
                int index = metaObject->indexOfProperty( propNameXml.toLatin1() );
                QMetaProperty property = metaObject->property(index);
                QVariant propValue;
                if( property.type() == QVariant::UserType )
                {
                    propValue = ple->property( propNameXml.toLatin1() );
                    if( propValue.canConvert<plv::Enum>() )
                    {
                        plv::Enum e = propValue.value<plv::Enum>();
                        e.setSelected( propValueXml );
                        propValue.setValue( e );

                    }
                }
                else
                {
                    propValue = convertData( property.type(), propValueXml );
                }
                ple->setProperty( propNameXml.toLatin1(), propValue );

                qDebug()<< "Found property with name: " << propNameXml
                        << " and value: " << propValue;

            }
        }
    }
}

void PipelineLoader::parseConnections( QDomNodeList* list, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    for( unsigned int i=0; i < list->length(); ++i )
    {
        QDomNode connectionNode = list->item( i );

        QDomElement sinkNode = connectionNode.firstChildElement( "sink" );
        if( sinkNode.isNull() )
        {
            // error
        }
        QDomElement sinkProcIdNode = sinkNode.firstChildElement( "processorId" );
        if( sinkProcIdNode.isNull() )
        {
            // error
        }

        QDomElement sinkPinIdNode = sinkNode.firstChildElement( "pinId" );
        if( sinkPinIdNode.isNull() )
        {
            // error
        }

        int sinkProcId = sinkProcIdNode.text().toInt();
        int sinkPinId  = sinkPinIdNode.text().toInt();

        QDomElement sourceNode =connectionNode.firstChildElement( "source" );
        if( sourceNode.isNull() )
        {
            // error
        }
        QDomElement sourceProcIdNode = sourceNode.firstChildElement( "processorId" );
        if( sourceProcIdNode.isNull() )
        {
            //error
        }
        QDomElement sourcePinIdNode = sourceNode.firstChildElement( "pinId" );
        if( sourcePinIdNode.isNull() )
        {
            //error
        }

        int sourceProcId = sourceProcIdNode.text().toInt();
        int sourcePinId  = sourcePinIdNode.text().toInt();

        qDebug() << "Found connection with source( " << sourceProcId << ","
                 << sourcePinId << ") and sink( " << sinkProcId << ","
                 << sinkPinId << ")";

        PipelineElement* sourceElement = pipeline->getElement( sourceProcId );
        PipelineElement* sinkElement   = pipeline->getElement( sinkProcId );

        if( sourceElement == 0 )
        {
            throw std::runtime_error( "Connection specified with invalid source id");
        }

        if( sinkElement == 0 )
        {
            throw std::runtime_error( "Connection specified with invalid sink id");
        }

        DataProducer* producer = dynamic_cast<DataProducer*>(sourceElement);
        DataConsumer* consumer = dynamic_cast<DataConsumer*>(sinkElement);

        if( producer == 0 )
        {
            throw std::runtime_error( "Source element is not a DataProducer");
        }

        if( consumer == 0 )
        {
            throw std::runtime_error( "Sink element is not a DataConsumer");
        }

        IOutputPin* iop = producer->getOutputPin( sourcePinId );
        IInputPin*  iip = consumer->getInputPin( sinkPinId );

        if( iop == 0 )
        {
            QString msg = tr("No output pin with id %1").arg(sourcePinId);
            throw std::runtime_error( msg.toStdString() );
        }

        if( iip == 0 )
        {
            QString msg = tr("No input pin with id %1").arg(sinkPinId);
            throw std::runtime_error( msg.toStdString() );
        }
        QString errstr;
        if( !pipeline->canConnectPins(iop, iip, errstr ))
        {
            throw std::runtime_error( tr("Cannot connect pins because : %1").arg(errstr).toStdString() );
        }
        pipeline->connectPins( iop, iip );
    }
}

int PipelineLoader::propertyIndex( QObject* qobject, const QString& name )
{
    return qobject->metaObject()->indexOfProperty(  name.toLatin1().constData() );
}

QVariant::Type PipelineLoader::propertyType( QObject* qobject, int index )
{
    if( index < 0 ) return( QVariant::Invalid );
    return( qobject->metaObject()->property(index).type() );
}

QVariant::Type PipelineLoader::propertyType( QObject* qobject, const QString& name )
{
    return( propertyType( qobject, propertyIndex( qobject, name ) ) );
}

void PipelineLoader::setProperty( QObject* qobject, int index, const QVariant& value )
{
    if( index >= 0 ) qobject->metaObject()->property(index).write( qobject, value );
}

void PipelineLoader::setProperty( QObject* qobject, const QString& name, const QVariant& value )
{
    setProperty( qobject, propertyIndex( qobject, name), value );
}

QVariant PipelineLoader::convertData( QVariant::Type type, const QString& data )
{
    switch( type )
    {
    case QVariant::Bool:
    {
        // accept either string "true" or "1"
        // everything else is considered false
        bool val = data.compare("true", Qt::CaseInsensitive) == 0;
        val = val || data.compare("1") == 0;
        return val;
    }
    case QVariant::Char:
    {
        return(data.size() > 0 ? data.at(0) : QChar());
    }
    case QVariant::ByteArray:
    {
        return(data.toLatin1());
    }
    case QVariant::Date:
    {
        return(QDateTime::fromString(data));
    }
    case QVariant::Double:
    {
        return(data.toDouble());
    }
    case QVariant::Int:
    {
        return(data.toInt());
    }
    case QVariant::LongLong:
    {
        return(data.toLongLong());
    }
    case QVariant::Time:
    {
        return(QTime::fromString(data));
    }
    case QVariant::UInt:
    {
        return(data.toUInt());
    }
    case QVariant::ULongLong:
    {
        return(data.toULongLong());
    }
    case QVariant::Url:
    {
        return(QUrl(data));
    }
    case QVariant::UserType:
    default:
        // unknown, just return as string
        return(data);
    }
}
