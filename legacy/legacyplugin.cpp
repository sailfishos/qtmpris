// -*- c++ -*-

/*
 *
 * Copyright (C) 2015-2021 Jolla Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "mprisplugin.h"

#include "legacympris_p.h"
#include "legacymprismanager_p.h"
#include "legacymprisplayer_p.h"

#include <qqml.h>

template<class T> QObject *api_factory(QQmlEngine *, QJSEngine *)
{
    return new T;
}

LegacyMprisPlugin::MprisPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

LegacyMprisPlugin::~MprisPlugin()
{
}

void LegacyMprisPlugin::registerTypes(const char *uri)
{
    qmlRegisterSingletonType<LegacyMpris>(uri, 1, 0, "Mpris", api_factory<LegacyMpris>);
    qmlRegisterType<LegacyMprisPlayer>(uri, 1, 0, "MprisPlayer");
    qmlRegisterType<LegacyMprisManager>(uri, 1, 0, "MprisManager");
}
