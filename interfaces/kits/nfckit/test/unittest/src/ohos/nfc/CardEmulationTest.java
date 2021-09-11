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

import ohos.aafwk.ability.Ability;
import ohos.bundle.ElementName;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.nfc.cardemulation.AidGroup;
import ohos.nfc.cardemulation.CardEmulation;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

/**
 * implement test cases for CardEmulation
 *
 * @author maolongxia
 */
public class CardEmulationTest {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "CardEmulationTest");

    /** testcase execution result */
    private static final int EXEC_FAILED = -1;
    private static final int EXEC_SUCCESS = 0;
    private static final int WAIT_TIME = 3 * 1000;
    private static int mExecResult = EXEC_SUCCESS;
    private CardEmulation mCardEmulation = null;
    private NfcController mNfcController = null;

    /**
     * set up every test case condition
     *
     * @throws Exception setup failed
     */
    @Before
    public void setUp() throws Exception {
        /**
         * @tc.setup: create the instance of cardemulation service if it is null
         */
        if (mCardEmulation == null) {
            HiLog.info(LABEL, "testCardEmulation setUp()");
            mNfcController = NfcController.getInstance(new ohos.aafwk.ability.Ability());
            mCardEmulation = CardEmulation.getInstance(mNfcController);
        }
        mExecResult = EXEC_SUCCESS;
    }

    /**
     * tear down every test case condition
     *
     * @throws Exception tearDown failed
     */
    @After
    public void tearDown() throws Exception {
        /**
         * @tc.teardown: destroy the instance of cardemulation service
         */
        mCardEmulation = null;
    }

    /**
     * @tc.name: testCardEmulation001
     * @tc.desc: create mCardEmulation and bind SA
     * @tc.require: SR000CSJPE AR000CSJRA
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation001() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);
    }

    /**
     * @tc.name: testCardEmulation002
     * @tc.desc: check isListenModeEnabled
     * @tc.require: SR000CSJPE AR000CSJRA
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation002() {
        try {
            /**
             * @tc.steps step1 check whether mCardEmulation is not null
             * @tc.expected step1 not null
             */
            Assert.assertNotNull(mCardEmulation);
            Thread.sleep(WAIT_TIME);
            /**
             * @tc.steps step2 query whether cardemulation is enabled or not
             * @tc.expected step2 disabled
             */
            mCardEmulation.setListenMode(CardEmulation.DISABLE_MODE_A_B);
            boolean isEnabled = mCardEmulation.isListenModeEnabled();
            if (isEnabled) {
                HiLog.info(LABEL, "testCardEmulation002 cardemulation is enabled.");
            }
        } catch (InterruptedException interruptedException) {
            mExecResult = EXEC_FAILED;
        }
        Assert.assertEquals(EXEC_SUCCESS, mExecResult);
    }

    /**
     * @tc.name: testCardEmulation003
     * @tc.desc: check isSupported
     * @tc.require: SR000CSJPE AR000CSJRA
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation003() {
        try {
            /**
             * @tc.steps step1 check whether mCardEmulation is not null
             * @tc.expected step1 not null
             */
            Assert.assertNotNull(mCardEmulation);
            Thread.sleep(WAIT_TIME);
            /**
             * @tc.steps step2 query whether card emulation is supported or not
             * @tc.expected step2 eSE is supported, SIM and HCE is unsupported.
             */
            boolean reply = mCardEmulation.isSupported(CardEmulation.FEATURE_HCE);
            HiLog.info(LABEL, "testCardEmulation003 is hce Supported, reply is %{public}b.", reply);
            Assert.assertEquals(reply, true);
            reply = mCardEmulation.isSupported(CardEmulation.FEATURE_UICC);
            HiLog.info(LABEL, "testCardEmulation003 is sim Supported, reply is %{public}b.", reply);
            Assert.assertEquals(reply, true);
            reply = mCardEmulation.isSupported(CardEmulation.FEATURE_ESE);
            HiLog.info(LABEL, "testCardEmulation003 is eSE Supported, reply is %{public}b.", reply);
            Assert.assertEquals(reply, true);
        } catch (InterruptedException interruptedException) {
            mExecResult = EXEC_FAILED;
        }
        Assert.assertEquals(EXEC_SUCCESS, mExecResult);
    }

    /**
     * @tc.name: testCardEmulation004
     * @tc.desc: check selection type for category
     * @tc.require: SR000CSJPE AR000CSJRA
     * @tc.author: wutong
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation004() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);

        /**
         * @tc.steps step1 check selection type for payment category is correct
         * @tc.steps step2 check selection type for others category is correct
         * @tc.expected both are true
         */
        int result = mCardEmulation.getSelectionType(CardEmulation.CATEGORY_PAYMENT);
        Assert.assertTrue(result == CardEmulation.SELECTION_TYPE_PREFER_DEFAULT);

        result = mCardEmulation.getSelectionType(CardEmulation.CATEGORY_OTHER);
        Assert.assertTrue(result == CardEmulation.SELECTION_TYPE_ASK_IF_CONFLICT);
    }

    /**
     * @tc.name: testCardEmulation005
     * @tc.desc: register card emulation prefer for app
     * @tc.require: SR000DOUDOU AR000DOUOV
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation005() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);

        /**
         * @tc.steps step2 register card emulation prefer for app
         */
        mCardEmulation.registerForegroundPreferred(new Ability(), new ElementName());
    }

    /**
     * @tc.name: testCardEmulation006
     * @tc.desc: unregister card emulation prefer for app
     * @tc.require: SR000DOUOR AR000DOUOT
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation006() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);

        /**
         * @tc.steps step2 unregister card emulation prefer for app
         */
        mCardEmulation.unregisterForegroundPreferred(new Ability());
    }

    /**
     * @tc.name: testCardEmulation007
     * @tc.desc: queries whether the app is currently the default handler for a specified aid
     * @tc.require: SR000DOUOR AR000DOUOT
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation007() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);

        /**
         * @tc.steps step2 queries whether the app is currently the default handler for a specified aid
         */
        String aid = "A0028321901280";
        mCardEmulation.isDefaultForAid(new ElementName(), aid);
    }

    /**
     * @tc.name: testCardEmulation008
     * @tc.desc: Registers a list of valid AIDs for payment type for the specified app
     * @tc.require: SR000DOUOR AR000DOUOT
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation008() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);

        /**
         * @tc.steps step2 Registers a list of valid AIDs for payment type for the specified app
         */
        List<String> aids1 = new ArrayList<String>();
        aids1.add(0, "A0028321901280");
        aids1.add(1, "A0028321901281");
        try {
            mCardEmulation.registerAids(new ElementName(), CardEmulation.CATEGORY_PAYMENT, aids1);
        } catch (IllegalArgumentException e) {
            HiLog.error(LABEL, "IllegalArgumentException when registerAids");
        }

        /**
         * @tc.steps step3 Registers a list of valid AIDs for other type for the specified app
         */
        List<String> aids2 = new ArrayList<String>();
        aids2.add(0, "A0028321901280");
        aids2.add(1, "A0028321901281");
        try {
            mCardEmulation.registerAids(new ElementName(), CardEmulation.CATEGORY_OTHER, aids2);
        } catch (IllegalArgumentException e) {
            HiLog.error(LABEL, "IllegalArgumentException when registerAids");
        }
    }


    /**
     * @tc.name: testCardEmulation009
     * @tc.desc: Registers a list of invalid AIDs for payment type for the specified app
     * @tc.require: SR000DOUOR AR000DOUOT
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation009() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);
        /**
         * @tc.steps step2 Registers a list of invalid AIDs for payment type for the specified app
         */
        List<String> aids1 = new ArrayList<String>();
        aids1.add(0, "A002832190128");
        aids1.add(1, "A0028321901281");
        try {
            mCardEmulation.registerAids(new ElementName(), CardEmulation.CATEGORY_PAYMENT, aids1);
        } catch (IllegalArgumentException e) {
            HiLog.error(LABEL, "IllegalArgumentException when registerAids");
        }

        /**
         * @tc.steps step3 Registers a list of invalid AIDs for payment type for the specified app
         */
        List<String> aids2 = new ArrayList<String>();
        aids2.add(0, "A002832190128*");
        aids2.add(1, "A0028321901281");
        try {
            mCardEmulation.registerAids(new ElementName(), CardEmulation.CATEGORY_PAYMENT, aids2);
        } catch (IllegalArgumentException e) {
            HiLog.error(LABEL, "IllegalArgumentException when registerAids");
        }

        /**
         * @tc.steps step4 Registers a list of invalid AIDs for payment type for the specified app
         */
        List<String> aids3 = new ArrayList<String>();
        aids3.add(0, "A002832190128G");
        aids3.add(1, "A0028321901281");
        try {
            mCardEmulation.registerAids(new ElementName(), CardEmulation.CATEGORY_PAYMENT, aids3);
        } catch (IllegalArgumentException e) {
            HiLog.error(LABEL, "IllegalArgumentException when registerAids");
        }
    }

    /**
     * @tc.name: testCardEmulation010
     * @tc.desc: removes a previously registered list of AIDs for a specified type for the specified app
     * @tc.require: SR000DOUOR AR000DOUOT
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation010() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);

        /**
         * @tc.steps step2 removes a previously registered list of AIDs for payment type for the specified app
         */
        mCardEmulation.removeAids(new ElementName(), CardEmulation.CATEGORY_PAYMENT);

        /**
         * @tc.steps step3 removes a previously registered list of AIDs for other type for the specified app
         */
        mCardEmulation.removeAids(new ElementName(), CardEmulation.CATEGORY_OTHER);
    }

    /**
     * @tc.name: testCardEmulation011
     * @tc.desc: gets a list of AIDs for a specific type for the specified app
     * @tc.require: SR000DOUOR AR000DOUOT
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation011() {
        /**
         * @tc.steps step1 check whether mCardEmulation is not null
         * @tc.expected step1 not null
         */
        Assert.assertNotNull(mCardEmulation);

        /**
         * @tc.steps step2 gets a list of AIDs for payment type for the specified app
         */
        try {
            mCardEmulation.getAids(new ElementName(), CardEmulation.CATEGORY_PAYMENT);
        } catch (IllegalArgumentException e) {
            HiLog.error(LABEL, "IllegalArgumentException when getAids");
        }

        /**
         * @tc.steps step3 gets a list of AIDs for other type for the specified app
         */
        try {
            mCardEmulation.getAids(new ElementName(), CardEmulation.CATEGORY_OTHER);
        } catch (IllegalArgumentException e) {
            HiLog.error(LABEL, "IllegalArgumentException when getAids");
        }
    }

    /**
     * @tc.name: testCardEmulation012
     * @tc.desc: get type of AidGroup
     * @tc.require: SR000DOUOR AR000DOUOT
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testCardEmulation012() {
        /**
         * @tc.steps step1 get type of AidGroup
         * @tc.expected step1 the type equals CATEGORY_PAYMENT
         */
        List<String> aids = new ArrayList<String>();
        aids.add(0, "A0028321901280");
        aids.add(1, "A0028321901281");
        String type = CardEmulation.CATEGORY_PAYMENT;
        AidGroup aidGroup1 = new AidGroup(aids, type);

        Assert.assertTrue(type.equals(aidGroup1.getType()));

        /**
         * @tc.steps step2 get type of AidGroup
         * @tc.expected step2 the type equals CATEGORY_OTHER
         */
        type = CardEmulation.CATEGORY_OTHER;
        AidGroup aidGroup2 = new AidGroup(aids, type);

        Assert.assertTrue(type.equals(aidGroup2.getType()));
    }
}
