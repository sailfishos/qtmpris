// -*- c++ -*-

/*!
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

#include <Mpris>
#include <MprisPlayer>
#include <MprisManager>
#include <MprisController>
#include <MprisMetaData>
#include "declarativemprisplayer_p.h"
#include "legacympris_p.h"
#include "legacymprismanager_p.h"
#include "legacymprisplayer_p.h"

#include <qqml.h>

template<class T> QObject *api_factory(QQmlEngine *, QJSEngine *)
{
    return new T;
}

MprisPlugin::MprisPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

MprisPlugin::~MprisPlugin()
{
}

void MprisPlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<Mpris>(uri, 2, 0, "Mpris", QStringLiteral("Mpris is a namespace object"));
    qmlRegisterType<DeclarativeMprisPlayer>(uri, 2, 0, "MprisPlayer");
    qmlRegisterType<MprisManager>(uri, 2, 0, "MprisManager");
    qmlRegisterUncreatableType<MprisMetaData>(uri, 2, 0, "MprisMetaData", QStringLiteral("MprisMetaData can't be created"));
    qmlRegisterUncreatableType<MprisController>(uri, 2, 0, "MprisController", QStringLiteral("MprisMetaData can't be created"));

    qmlRegisterSingletonType<LegacyMpris>(uri, 1, 0, "Mpris", api_factory<LegacyMpris>);
    qmlRegisterType<LegacyMprisPlayer>(uri, 1, 0, "MprisPlayer");
    qmlRegisterType<LegacyMprisManager>(uri, 1, 0, "MprisPlayer");
}
