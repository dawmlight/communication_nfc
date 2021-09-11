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

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

/**
 * implement test cases for class NfcController
 *
 * @author yanghan
 */
public class NfcControllerTest {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "NfcControllerTest");

    /** testcase execution result */
    private static final int EXEC_FAILED = -1;
    private static final int EXEC_SUCCESS = 0;
    private static final int WAIT_TIME = 3 * 1000;
    private static int execResult = EXEC_SUCCESS;
    private NfcController mNfcController = null;

    /** set up every test case condition. */
    @Before
    public void setUp() throws Exception {
        /**
         * @tc.setup: create the instance of NFC
         */
        mNfcController = NfcController.getInstance(new ohos.aafwk.ability.Ability());
        execResult = EXEC_SUCCESS;
    }

    /** tear down every test case condition. */
    @After
    public void tearDown() throws Exception {
        /**
         * @tc.teardown: destroy the instance of NFC
         */
        mNfcController = null;
    }

    /**
     * @tc.name: testNfcController001
     * @tc.desc: open NFC and expect success
     * @tc.require: AR000CSEHN
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testNfcController001() {
        try {
            /**
             * @tc.steps step1 open NFC
             * @tc.expected step1 request NFC is opened
             */
            mNfcController.getContext();
            boolean isSuccess = mNfcController.openNfc();
            Assert.assertEquals(isSuccess, true);
            Thread.sleep(WAIT_TIME);

            /**
             * @tc.steps step2 check whether NFC is open
             * @tc.expected step2 true
             */
            boolean isOpen = mNfcController.isNfcOpen();
            Assert.assertEquals(isOpen, true);

            /**
             * @tc.steps step3 get current NFC state
             * @tc.expected step3 NFC is on
             */
            int state = mNfcController.getNfcState();
            HiLog.info(LABEL, "testNfcController001 getNfcState. %{public}d", state);
            Assert.assertEquals(state, NfcController.STATE_ON);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }
        Assert.assertEquals(EXEC_SUCCESS, execResult);
    }

    /**
     * @tc.name: testNfcController002
     * @tc.desc: NFC is available.
     * @tc.require: AR000CSEHN AR000CSJ3L
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testNfcController002() {
        /**
         * @tc.steps step1 check whether support NFC
         * @tc.expected step1 true
         */
        boolean isAvailable = mNfcController.isNfcAvailable();
        Assert.assertEquals(isAvailable, true);
    }

    /**
     * @tc.name: testNfcController003
     * @tc.desc: close NFC, then query NFC state
     * @tc.require: AR000CSEHN
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testNfcController003() {
        try {
            /**
             * @tc.steps step1 close NFC
             * @tc.expected step1 request is statified
             */
            boolean isSuccess = mNfcController.closeNfc();
            Assert.assertEquals(isSuccess, true);
            Thread.sleep(WAIT_TIME);

            /**
             * @tc.steps step2 check whether NFC is open
             * @tc.expected step2 false
             */
            boolean isOpen = mNfcController.isNfcOpen();
            Assert.assertEquals(isOpen, false);

            /**
             * @tc.steps step3 get current NFC state
             * @tc.expected step3 NFC is off
             */
            int state = mNfcController.getNfcState();
            Assert.assertEquals(state, NfcController.STATE_OFF);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }
        Assert.assertEquals(EXEC_SUCCESS, execResult);
    }
}
