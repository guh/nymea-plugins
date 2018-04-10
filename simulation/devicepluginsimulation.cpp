/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2017 Bernhard Trinnes <bernhard.trinnes@guh.io>          *
 *  Copyright (C) 2018 Simon Stürz <simon.stuerz@guh.io>                   *
 *                                                                         *
 *  This file is part of nymea.                                            *
 *                                                                         *
 *  nymea is free software: you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  nymea is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with nymea. If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "devicepluginsimulation.h"
#include "plugininfo.h"

#include <QColor>
#include <QDateTime>

DevicePluginSimulation::DevicePluginSimulation()
{

}

DevicePluginSimulation::~DevicePluginSimulation()
{
    hardwareManager()->pluginTimerManager()->unregisterTimer(m_pluginTimer20Seconds);
    hardwareManager()->pluginTimerManager()->unregisterTimer(m_pluginTimer5Min);
}

void DevicePluginSimulation::init()
{
    // Seed the random generator with current time
    qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);

    // Change some values every 20 seconds
    m_pluginTimer20Seconds = hardwareManager()->pluginTimerManager()->registerTimer(20);
    connect(m_pluginTimer20Seconds, &PluginTimer::timeout, this, &DevicePluginSimulation::onPluginTimer20Seconds);

    // Change some values every 5 min
    m_pluginTimer5Min = hardwareManager()->pluginTimerManager()->registerTimer(300);
    connect(m_pluginTimer5Min, &PluginTimer::timeout, this, &DevicePluginSimulation::onPluginTimer5Minutes);
}

DeviceManager::DeviceSetupStatus DevicePluginSimulation::setupDevice(Device *device)
{
    qCDebug(dcSimulation()) << "Set up device" << device->name();
    return DeviceManager::DeviceSetupStatusSuccess;
}

DeviceManager::DeviceError DevicePluginSimulation::executeAction(Device *device, const Action &action)
{
    // Check the DeviceClassId for "Simple Button"
    if (device->deviceClassId() == simpleButtonDeviceClassId ) {

        // check if this is the "press" action
        if (action.actionTypeId() == simpleButtonTriggerActionTypeId) {

            // Emit the "button pressed" event
            qCDebug(dcSimulation()) << "Emit button pressed event for" << device->name();
            Event event(simpleButtonPressedEventTypeId, device->id());
            emit emitEvent(event);

            return DeviceManager::DeviceErrorNoError;
        }
        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    // Check the DeviceClassId for "Alternative Button"
    if (device->deviceClassId() == alternativeButtonDeviceClassId) {

        // check if this is the "set power" action
        if (action.actionTypeId() == alternativeButtonPowerActionTypeId) {

            // get the param value
            Param powerParam = action.param(alternativeButtonPowerActionParamTypeId);
            bool power = powerParam.value().toBool();

            qCDebug(dcSimulation()) << "Set power" << power << "for button" << device->name();

            // Set the "power" state
            device->setStateValue(alternativeButtonPowerStateTypeId, power);

            return DeviceManager::DeviceErrorNoError;
        }
        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    if (device->deviceClassId() == heatingDeviceClassId) {

        // check if this is the "set power" action
        if (action.actionTypeId() == heatingPowerActionTypeId) {

            // get the param value
            Param powerParam = action.param(heatingPowerActionParamTypeId);
            bool power = powerParam.value().toBool();
            qCDebug(dcSimulation()) << "Set power" << power << "for heating device" << device->name();
            device->setStateValue(heatingPowerStateTypeId, power);
            return DeviceManager::DeviceErrorNoError;

        } else if (action.actionTypeId() == heatingTargetTemperatureActionTypeId) {

            // get the param value
            Param temperatureParam = action.param(heatingTargetTemperatureActionParamTypeId);
            int temperature = temperatureParam.value().toInt();

            qCDebug(dcSimulation()) << "Set target temperature" << temperature << "for heating device" << device->name();

            device->setStateValue(heatingTargetTemperatureStateTypeId, temperature);
            return DeviceManager::DeviceErrorNoError;
        }
        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    if (device->deviceClassId() == evChargerDeviceClassId){

        if (action.actionTypeId() == evChargerPowerActionTypeId){
            // get the param value
            Param powerParam = action.param(evChargerPowerActionParamTypeId);
            bool power = powerParam.value().toBool();

            qCDebug(dcSimulation()) << "Set power" << power << "for heating device" << device->name();

            device->setStateValue(evChargerPowerStateTypeId, power);
            return DeviceManager::DeviceErrorNoError;

        } else if(action.actionTypeId() == evChargerCurrentActionTypeId){
            // get the param value
            Param currentParam = action.param(evChargerCurrentActionParamTypeId);
            int current = currentParam.value().toInt();
            qCDebug(dcSimulation()) << "Set current" << current << "for EV Charger device" << device->name();
            device->setStateValue(evChargerCurrentStateTypeId, current);
            return DeviceManager::DeviceErrorNoError;
        }
        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    if(device->deviceClassId() == socketDeviceClassId){

        if(action.actionTypeId() == socketPowerActionTypeId){
            // get the param value
            Param powerParam = action.param(socketPowerActionParamTypeId);
            bool power = powerParam.value().toBool();
            // Set the "power" state
            qCDebug(dcSimulation()) << "Set power" << power << "for socket device" << device->name();
            device->setStateValue(socketPowerStateTypeId, power);
            return DeviceManager::DeviceErrorNoError;
        }
        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    if(device->deviceClassId() == colorBulbDeviceClassId){

        if(action.actionTypeId() == colorBulbBrightnessActionTypeId){
            int brightness = action.param(colorBulbBrightnessActionParamTypeId).value().toInt();
            qCDebug(dcSimulation()) << "Set brightness" << brightness << "for color bulb device" << device->name();
            device->setStateValue(colorBulbBrightnessStateTypeId, brightness);
            return DeviceManager::DeviceErrorNoError;

        } else if (action.actionTypeId() == colorBulbColorTemperatureActionTypeId){
            int temperature = action.param(colorBulbColorTemperatureActionParamTypeId).value().toInt();
            qCDebug(dcSimulation()) << "Set color temperature" << temperature << "for color bulb device" << device->name();
            device->setStateValue(colorBulbColorTemperatureStateTypeId, temperature);
            return DeviceManager::DeviceErrorNoError;

        } else if (action.actionTypeId() == colorBulbColorActionTypeId) {
            QColor color = action.param(colorBulbColorActionParamTypeId).value().value<QColor>();
            qCDebug(dcSimulation()) << "Set color" << color << "for color bulb device" << device->name();
            device->setStateValue(colorBulbColorStateTypeId, color);
            return DeviceManager::DeviceErrorNoError;

        } else if (action.actionTypeId() == colorBulbPowerActionTypeId) {
            bool power = action.param(colorBulbPowerActionParamTypeId).value().toBool();
            qCDebug(dcSimulation()) << "Set power" << power << "for color bulb device" << device->name();
            device->setStateValue(colorBulbPowerStateTypeId, power);
            return DeviceManager::DeviceErrorNoError;
        }

        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    if (device->deviceClassId() == heatingRodDeviceClassId) {

        if (action.actionTypeId() == heatingRodPowerActionTypeId) {
            bool power = action.param(heatingRodPowerActionParamTypeId).value().toBool();
            qCDebug(dcSimulation()) << "Set power" << power << "for heating rod device" << device->name();
            device->setStateValue(heatingRodPowerStateTypeId, power);
            return DeviceManager::DeviceErrorNoError;
        } else if (action.actionTypeId() == heatingRodWaterTemperatureActionTypeId) {
            int temperature = action.param(heatingRodWaterTemperatureActionParamTypeId).value().toInt();
            qCDebug(dcSimulation()) << "Set water temperature" << temperature << "for heating rod device" << device->name();
            device->setStateValue(heatingRodWaterTemperatureStateTypeId, temperature);
            return DeviceManager::DeviceErrorNoError;
        } else if (action.actionTypeId() == heatingRodMaxPowerActionTypeId) {
            double maxPower = action.param(heatingRodMaxPowerActionParamTypeId).value().toDouble();
            qCDebug(dcSimulation()) << "Set max power" << maxPower << "for heating rod device" << device->name();
            device->setStateValue(heatingRodMaxPowerStateTypeId, maxPower);
            return DeviceManager::DeviceErrorNoError;
        }

        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    if (device->deviceClassId() == batteryDeviceClassId) {
        if (action.actionTypeId() == batteryMaxChargingActionTypeId) {
            int maxCharging = action.param(batteryMaxChargingActionParamTypeId).value().toInt();
            device->setStateValue(batteryMaxChargingStateTypeId, maxCharging);
            qCDebug(dcSimulation()) << "Set max charging power" << maxCharging << "for battery device" << device->name();
            device->setStateValue(batteryChargingStateTypeId, maxCharging);
            return DeviceManager::DeviceErrorNoError;
        }
        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    if (device->deviceClassId() == waterValveDeviceClassId) {
        if (action.actionTypeId() == waterValvePowerActionTypeId) {
            bool power = action.param(waterValvePowerActionParamTypeId).value().toBool();
            device->setStateValue(waterValvePowerStateTypeId, power);
            return DeviceManager::DeviceErrorNoError;
        }
        return DeviceManager::DeviceErrorActionTypeNotFound;
    }

    return DeviceManager::DeviceErrorDeviceClassNotFound;
}

int DevicePluginSimulation::generateRandomIntValue(int min, int max)
{
    int value = ((qrand() % ((max + 1) - min)) + min);
    // qCDebug(dcSimulation()) << "Generateed random int value: [" << min << ", " << max << "] -->" << value;
    return value;
}

double DevicePluginSimulation::generateRandomDoubleValue(double min, double max)
{
    double value = generateRandomIntValue(static_cast<int>(min * 10), static_cast<int>(max * 10)) / 10.0;
    // qCDebug(dcSimulation()) << "Generated random double value: [" << min << ", " << max << "] -->" << value;
    return value;
}

bool DevicePluginSimulation::generateRandomBoolValue()
{
    bool value = static_cast<bool>(generateRandomIntValue(0, 1));
    // qCDebug(dcSimulation()) << "Generated random bool value:" << value;
    return value;
}

void DevicePluginSimulation::onPluginTimer20Seconds()
{
    foreach (Device *device, myDevices()) {
        if (device->deviceClassId() == temperatureSensorDeviceClassId) {
            // Temperature sensor
            device->setStateValue(temperatureSensorTemperatureStateTypeId, generateRandomDoubleValue(18, 23));
            device->setStateValue(temperatureSensorHumidityStateTypeId, generateRandomIntValue(40, 55));
            device->setStateValue(temperatureSensorBatteryLevelStateTypeId, generateRandomIntValue(25, 40));
            device->setStateValue(temperatureSensorBatteryCriticalStateTypeId, device->stateValue(temperatureSensorBatteryLevelStateTypeId).toInt() <= 30);
            device->setStateValue(temperatureSensorConnectedStateTypeId, true);
        } else if (device->deviceClassId() == motionDetectorDeviceClassId) {
            // Motion detector
            device->setStateValue(motionDetectorActiveStateTypeId, generateRandomBoolValue());
            device->setStateValue(motionDetectorBatteryLevelStateTypeId, generateRandomIntValue(25, 40));
            device->setStateValue(motionDetectorBatteryCriticalStateTypeId, device->stateValue(motionDetectorBatteryLevelStateTypeId).toInt() <= 30);
            device->setStateValue(motionDetectorConnectedStateTypeId, true);
        } else if (device->deviceClassId() == gardenSensorDeviceClassId) {
            // Garden sensor
            device->setStateValue(gardenSensorTemperatureStateTypeId, generateRandomDoubleValue(20, 23));
            device->setStateValue(gardenSensorSoilMoistureStateTypeId, generateRandomIntValue(40, 60));
            device->setStateValue(gardenSensorIlluminanceStateTypeId, generateRandomIntValue(20, 80));
            device->setStateValue(gardenSensorBatteryLevelStateTypeId, generateRandomIntValue(25, 90));
            device->setStateValue(gardenSensorBatteryCriticalStateTypeId, device->stateValue(gardenSensorBatteryLevelStateTypeId).toDouble() <= 30);
            device->setStateValue(gardenSensorConnectedStateTypeId, true);
        } else if(device->deviceClassId() == netatmoIndoorDeviceClassId) {
            // Netatmo
            device->setStateValue(netatmoIndoorUpdateTimeStateTypeId, QDateTime::currentDateTime().toTime_t());
            device->setStateValue(netatmoIndoorHumidityStateTypeId, generateRandomIntValue(35, 45));
            device->setStateValue(netatmoIndoorTemperatureStateTypeId, generateRandomIntValue(20, 25));
            device->setStateValue(netatmoIndoorPressureStateTypeId, generateRandomIntValue(1003, 1008));
            device->setStateValue(netatmoIndoorNoiseStateTypeId, generateRandomIntValue(40, 80));
            device->setStateValue(netatmoIndoorWifiStrengthStateTypeId, generateRandomIntValue(85, 95));
        }
    }
}

void DevicePluginSimulation::onPluginTimer5Minutes()
{
    foreach (Device *device, myDevices()) {
        if(device->deviceClassId() == netatmoIndoorDeviceClassId) {
            // Note: should change between > 1000 co2 < 1000 for showcase, please do not change this behaviour
            int currentValue = device->stateValue(netatmoIndoorCo2StateTypeId).toInt();
            if (currentValue < 1000) {
                device->setStateValue(netatmoIndoorCo2StateTypeId, generateRandomIntValue(1001, 1010));
            } else {
                device->setStateValue(netatmoIndoorCo2StateTypeId, generateRandomIntValue(950, 999));
            }
        }
    }
}
