/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package ohos.nfc.cardemulation;

import ohos.aafwk.ability.Ability;
import ohos.aafwk.content.Intent;
import ohos.rpc.IRemoteObject;

/**
 * An abstract class that is inherited by the NFC off-host application.
 *
 * <p>The NFC off-host application inherits from this abstract class then the NFC service can obtain the application
 * installation information and connect to services of the application.
 *
 * @since 3
 */
public abstract class OffHostService extends Ability {
    /**
     * Indicates the service name.
     *
     * @since 3
     */
    public static final String SERVICE_NAME = "ohos.nfc.cardemulation.action.OFF_HOST_SERVICE";

    /**
     * Indicates the name of the service meta-data element.
     *
     * @since 3
     */
    public static final String META_DATA_NAME = "ohos.nfc.cardemulation.data.off_host_service";

    /**
     * Connects to the host service to obtain a remote service.
     *
     * <p>If the host service has been connected, this method can be used to obtain a remote service for callback.
     *
     * @param intent Indicates the intent of the remote service for callback.
     * @return Returns a remote service.
     * @since 3
     */
    @Override
    public abstract IRemoteObject onConnect(Intent intent);
}
