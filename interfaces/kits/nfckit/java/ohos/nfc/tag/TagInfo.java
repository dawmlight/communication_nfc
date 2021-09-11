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

package ohos.nfc.tag;

import ohos.interwork.utils.PacMapEx;
import ohos.aafwk.content.IntentParams;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.utils.system.SystemCap;

import java.util.Arrays;
import java.util.Optional;

/**
 * Provides tag information.
 *
 * <p>This class provides the technology a tag supports, for example, NFC-A. Applications can create
 * different tags based on the supported technology.
 *
 * @Syscap {@link SystemCapability.Communication#NFC}
 * @since 1
 */
@SystemCap("SystemCapability.Communication.NFC")
public class TagInfo implements Sequenceable {
    /**
     * Indicates the key used to obtain the extraneous information of technology supported by the tag.
     *
     * @since 1
     */
    public static final String EXTRA_TAG_EXTRAS = "extra_nfc_TAG_EXTRAS";

    /**
     * Indicates the key used to obtain the tag handle.
     *
     * @since 1
     */
    public static final String EXTRA_TAG_HANDLE = "extra_nfc_TAG_HANDLE";

    private static int LIMIT_FOR_DATA = 1024;

    private byte[] mTagId = null;
    private int[] mTagSupportedProfiles = null;
    private PacMapEx[] mProfileExtras = null;
    private int mTagHandle;

    /**
     * A constructor used to create a {@code TagInfo} instance.
     *
     * @param id Indicates unique or random ID for different tag.
     * @param profiles Indicates all supported protocols or technologies.
     * @param extras Indicates extra information for each profile.
     * @param tagHandle Indicates tag handle given from service for this tag.
     * @hide
     */
    public TagInfo(byte[] id, int[] profiles, PacMapEx[] extras, int tagHandle) {
        if (id != null && id.length < LIMIT_FOR_DATA) {
            mTagId = Arrays.copyOf(id, id.length);
        }
        if (profiles != null && profiles.length < LIMIT_FOR_DATA) {
            mTagSupportedProfiles = Arrays.copyOf(profiles, profiles.length);
        }
        if (extras != null && extras.length < LIMIT_FOR_DATA) {
            mProfileExtras = Arrays.copyOf(extras, extras.length);
        }
        mTagHandle = tagHandle;
    }

    @Override
    public boolean marshalling(Parcel out) {
        if (out == null) {
            return false;
        }
        if (mTagId != null && mTagId.length != 0) {
            out.writeByteArray(mTagId);
        }
        if (mTagSupportedProfiles != null && mTagSupportedProfiles.length != 0) {
            out.writeIntArray(mTagSupportedProfiles);
        }
        if (mProfileExtras != null && mProfileExtras.length != 0) {
            out.writeSequenceableArray(mProfileExtras);
        }
        out.writeInt(mTagHandle);
        return true;
    }

    @Override
    public boolean unmarshalling(Parcel in) {
        if (in == null) {
            return false;
        }
        int length = in.readInt();
        mProfileExtras = new PacMapEx[length];
        for (int i = 0; i < length; i++) {
            mProfileExtras[i] = new PacMapEx();
            in.readPacMapEx(mProfileExtras[i]);
        }
        this.mTagHandle = in.readInt();
        int tagIdLength = in.readInt();
        this.mTagId = new byte[tagIdLength];
        for (int i = 0; i < tagIdLength; i++) {
            mTagId[i] = in.readByte();
        }
        int tagSupportedProfilesLength = in.readInt();
        this.mTagSupportedProfiles = new int[tagSupportedProfilesLength];
        for (int i = 0; i < tagSupportedProfilesLength; i++) {
            mTagSupportedProfiles[i] = in.readInt();
        }
        return true;
    }

    /**
     * Obtains the ID of this tag.
     *
     * @return Returns the ID of this tag.
     * @since 1
     */
    public byte[] getTagId() {
        if (mTagId != null) {
            return Arrays.copyOf(mTagId, mTagId.length);
        }
        return new byte[0];
    }

    /**
     * Obtains the protocol or technology supported by this tag.
     *
     * @return Returns the protocol or technology supported by this tag.
     * @since 1
     */
    public int[] getTagSupportedProfiles() {
        if (mTagSupportedProfiles != null) {
            return Arrays.copyOf(mTagSupportedProfiles, mTagSupportedProfiles.length);
        }
        return new int[0];
    }

    /**
     * Gets the handle given from service for this tag.
     *
     * @return Returns tag handle.
     * @hide
     */
    public int getTagHandle() {
        return mTagHandle;
    }

    /**
     * Obtains extra information for given profile.
     *
     * @return Returns extra information.
     * @hide
     */
    public Optional<PacMapEx> getProfileExtras(int profile) {
        int idx = -1;
        if (mTagSupportedProfiles != null) {
            for (int i = 0; i < mTagSupportedProfiles.length; i++) {
                if (mTagSupportedProfiles[i] == profile) {
                    idx = i;
                    break;
                }
            }
        }
        if (idx < 0) {
            return Optional.empty();
        }

        return Optional.of(mProfileExtras[idx]);
    }

    /**
     * Checks whether a tag supports a specified protocol or technology.
     *
     * @param profile Indicates the specified protocol or technology.
     * @return Returns {@code true} if the tag supports the specified protocol or technology;
     * returns {@code false} otherwise.
     * @since 1
     */
    public boolean isProfileSupported(int profile) {
        if (mTagSupportedProfiles != null) {
            for (int prof : mTagSupportedProfiles) {
                if (prof == profile) {
                    return true;
                }
            }
        }
        return false;
    }
}
