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

package ohos.nfc;

import ohos.utils.Parcel;
import ohos.utils.Sequenceable;

/**
 * Provides the profiles to perform matching for dispatching.
 *
 * @hide
 * @since 3
 */
public class ProfileParcel implements Sequenceable {
    private static int LIMIT_FOR_DATA = 1024;

    private String[][] mProfiles;

    public ProfileParcel(String[]... profiles) {
        mProfiles = profiles;
    }

    /**
     * Gets the profiles to perform matching for dispatching.
     *
     * @return returns the profiles to perform matching for dispatching.
     * @since 3
     */
    public String[][] getProfiles() {
        return mProfiles;
    }

    @Override
    public boolean marshalling(Parcel out) {
        if (out == null) {
            return false;
        }
        if (mProfiles == null || mProfiles.length == 0) {
            out.writeInt(0);
            return false;
        }
        int length = mProfiles.length;
        out.writeInt(length);
        for (int i = 0; i < length; i++) {
            String[] profile = mProfiles[i];
            out.writeStringArray(profile);
        }
        return true;
    }

    @Override
    public boolean unmarshalling(Parcel in) {
        if (in == null) {
            return false;
        }
        int length = in.readInt();
        if (length == 0 || length > LIMIT_FOR_DATA) {
            return false;
        }

        String[][] profiles = new String[length][];
        for (int i = 0; i < length; i++) {
            profiles[i] = in.readStringArray();
        }
        this.mProfiles = profiles;
        return true;
    }
}