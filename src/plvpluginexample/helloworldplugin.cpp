#include "helloworldplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "helloworldprocessor.h"

#include <plvcore/PipelineElementFactory.h>

using namespace plv;

HelloWorldPlugin::HelloWorldPlugin()
{
}

HelloWorldPlugin::~HelloWorldPlugin()
{
}

void HelloWorldPlugin::onLoad()
{
    plvRegisterPipelineElement<HelloWorldProcessor>();
}
