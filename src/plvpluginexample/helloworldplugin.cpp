#include "helloworldplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "helloworldprocessor.h"

#include <plvcore/PipelineElementFactory.h>

using namespace plv;

HelloWorldPlugin::HelloWorldPlugin()
{
    qDebug() << "HelloWorldPlugin constructor";
}

HelloWorldPlugin::~HelloWorldPlugin()
{
    qDebug() << "HelloWorldPlugin destructor";
}

void HelloWorldPlugin::onLoad()
{
    qDebug() << "HelloWorldPlugin onLoad";
    plvRegisterPipelineElement<HelloWorldProcessor>();
}
