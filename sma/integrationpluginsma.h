/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; version 3. This project is distributed in the hope that
* it will be useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef INTEGRATIONPLUGINSMA_H
#define INTEGRATIONPLUGINSMA_H

#include "integrations/integrationplugin.h"
#include "plugintimer.h"
#include "sunnywebbox.h"
#include "sunnywebboxcommunication.h"
#include "discovery.h"

#include <QDebug>
#include <QHostAddress>
#include <QUdpSocket>


class IntegrationPluginSma: public IntegrationPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "io.nymea.IntegrationPlugin" FILE "integrationpluginsma.json")
    Q_INTERFACES(IntegrationPlugin)

public:
    explicit IntegrationPluginSma();

    void discoverThings(ThingDiscoveryInfo *info) override;
    void setupThing(ThingSetupInfo *info) override;
    void postSetupThing(Thing *thing) override;
    void executeAction(ThingActionInfo *info) override;
    void thingRemoved(Thing *thing) override;

private slots:
    void onRefreshTimer();

    void onConnectedChanged(bool connected);
    void onPlantOverviewReceived(const QString &messageId, SunnyWebBox::Overview overview);
    void onDevicesReceived(const QString &messageId, QList<SunnyWebBox::Device> devices);
    void onParametersReceived(const QString &messageId, const QString &deviceKey, const QList<SunnyWebBox::Parameter> &parameters);
    void onProcessDataReceived(const QString &messageId, const QString &deviceKey, const QHash<QString, QVariant> &channels);
    void onParameterChannelsReceived(const QString &messageId, const QString &deviceKey, QStringList parameterChannels);

private:
    PluginTimer *m_refreshTimer = nullptr;
    QHash<Thing *, SunnyWebBox *> m_sunnyWebBoxes;
    QHash<QString, ThingSetupInfo *> m_asyncSetup;
    QHash<QString, ThingActionInfo *> m_asyncActions;
    SunnyWebBoxCommunication *m_sunnyWebBoxCommunication = nullptr;

    void getData(Thing *thing);
};

#endif // INTEGRATIONPLUGINSMA_H