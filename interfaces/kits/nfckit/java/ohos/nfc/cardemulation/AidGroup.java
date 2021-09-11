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

import ohos.utils.Parcel;
import ohos.utils.Sequenceable;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/**
 * Provides AID group.
 *
 * <p>This class provides the technology a tag supports, for example, NFC-A. Applications can create
 * different tags based on the supported technology.
 *
 * @hide
 * @since 3
 */
public class AidGroup implements Sequenceable {
    /**
     * The maximum size of AIDs that can be present in any one group.
     */
    public static final int MAX_SIZE_AIDS = 256;

    private List<String> mAids = null;
    private String mType;

    /**
     * A constructor used to create an {@code AidGroup} instance.
     */
    public AidGroup() {
        mAids = new ArrayList<String>();
        mType = "";
    }

    /**
     * A constructor used to create an {@code AidGroup} instance.
     *
     * @param aids Indicates the list of AIDs present in the group.
     * @param type Indicates the type of this group, for example {@link CardEmulation#CATEGORY_PAYMENT}.
     */
    public AidGroup(List<String> aids, String type) {
        if (aids == null || aids.size() == 0) {
            throw new NullPointerException("aids is null");
        }
        if (aids.size() > MAX_SIZE_AIDS) {
            throw new IllegalArgumentException("Too many aids");
        }
        for (String aid : aids) {
            if (!CardEmulation.isAidValid(aid)) {
                throw new IllegalArgumentException("aid is not valid");
            }
        }

        if (CardEmulation.CATEGORY_PAYMENT.equals(type) || CardEmulation.CATEGORY_OTHER.equals(type)) {
            this.mType = type;
        } else {
            this.mType = CardEmulation.CATEGORY_OTHER;
        }

        this.mAids = new ArrayList<String>(aids.size());
        for (String aid : aids) {
            this.mAids.add(aid.toUpperCase(Locale.ROOT));
        }
    }

    @Override
    public boolean marshalling(Parcel out) {
        if (out == null) {
            return false;
        }
        out.writeString(mType);
        out.writeInt(mAids.size());
        if (mAids.size() > 0) {
            out.writeStringList(mAids);
        }
        return true;
    }

    @Override
    public boolean unmarshalling(Parcel in) {
        if (in == null) {
            return false;
        }
        this.mType = in.readString();
        int groupSize = in.readInt();
        if (groupSize > 0) {
            this.mAids = in.readStringList();
        }
        return true;
    }

    /**
     * Obtains the list of AIDs of the aidGroup.
     *
     * @return Returns the list of AIDs.
     * @since 3
     */
    public List<String> getAids() {
        if (mAids == null || mAids.size() == 0) {
            return new ArrayList<String>(0);
        }
        List<String> aids = new ArrayList<String>(mAids.size());
        aids.addAll(mAids);
        return aids;
    }

    /**
     * Obtains the type of AIDs.
     *
     * @return Returns the type of AIDs.
     * @since 3
     */
    public String getType() {
        return mType;
    }
}
