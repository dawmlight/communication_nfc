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

import ohos.event.intentagent.IntentAgent;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.interwork.utils.PacMapEx;
import ohos.nfc.tag.TagInfo;
import ohos.rpc.IRemoteObject;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.TestTarget;
import ohos.unittest.Type;
import ohos.unittest.log.Logger;
import ohos.unittest.perf.BaseLine;
import ohos.unittest.perf.PerfVerify;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

/**
 * Description: performance test for NfcController class.
 *
 * @author liujiawei
 */
public final class NfcPerformanceTest {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE,
        NfcKitsUtils.NFC_DOMAIN_ID, "NfcPerformanceTest");
    private static String baseLineXmlName = "nfc_baseline.xml";
    private static int TEST_NUM = 1000;
    private static BaseLine baseLine;
    private static NfcController sNfcController = null;
    private class CardReader implements NfcController.ReaderModeCallback {
        @Override
        public void onTagDiscovered(TagInfo tagInfo) {
            HiLog.info(LABEL, "CardReader, onTagDiscovered");
        }

        @Override
        public IRemoteObject asObject() {
            return null;
        }
    }

    private CardReader mCardReader;

    /**
     * Init test requirements.
     */
    @BeforeClass
    public static void beforeClass() {
        HiLog.info(LABEL, "init NFC Performance test");
        sNfcController = NfcController.getInstance(new ohos.aafwk.ability.Ability());
        String baseLineConfigFile = "/data/test/" + baseLineXmlName;
        baseLine = new BaseLine(baseLineConfigFile);
    }

    /**
     * Destroy test objects.
     */
    @AfterClass
    public static void afterClass() {
        HiLog.info(LABEL, "tear down NFC Performance test");
        sNfcController = null;
        baseLineXmlName = null;
        baseLine = null;
    }

    /**
     * @tc.name: testNfcOpen001
     * @tc.desc: test open NFC performance
     * @tc.require: AR000CSEHN
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.SECU)
    @TestTarget(className = "ohos.nfc.NfcController", methodName = "public boolean openNfc()")
    public void testNfcOpen001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < TEST_NUM; index++) {
            long startTime = System.nanoTime();

            // Enable the NFC.
            sNfcController.openNfc();

            long endTime = System.nanoTime();
            times.add(endTime - startTime);
        }
        Double asDouble = times.stream().mapToLong(Long::longValue).average().getAsDouble();
        verify.expectSmaller(changeNSToUS(asDouble), 1);
    }

    /**
     * @tc.name: testNfcClose001
     * @tc.desc: test close NFC performance
     * @tc.require: AR000CSEHN
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.SECU)
    public void testNfcClose001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < TEST_NUM; index++) {
            long startTime = System.nanoTime();

            // Invoke disable the NFC interface.
            sNfcController.closeNfc();

            long endTime = System.nanoTime();
            times.add(endTime - startTime);
        }
        Double asDouble = times.stream().mapToLong(Long::longValue).average().getAsDouble();
        verify.expectSmaller(changeNSToUS(asDouble), 1);
    }

    /**
     * @tc.name: testGetNfcState001
     * @tc.desc: test get NFC state performance
     * @tc.require: AR000CSEHN
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.SECU)
    public void testGetNfcState001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < TEST_NUM; index++) {
            long startTime = System.nanoTime();

            // Invoke get the NFC state interface.
            sNfcController.getNfcState();

            long endTime = System.nanoTime();
            times.add(endTime - startTime);
        }
        Double asDouble = times.stream().mapToLong(Long::longValue).average().getAsDouble();
        verify.expectSmaller(changeNSToUS(asDouble), 1);
    }

    /**
     * @tc.name: testIsNfcAvailable001
     * @tc.desc: test is NFC available performance
     * @tc.require: AR000CSEHN
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.SECU)
    public void testIsNfcAvailable001() {
        PerfVerify verify = new PerfVerify(baseLine);
        List<Long> times = new ArrayList<>();
        for (int index = 0; index < TEST_NUM; index++) {
            long startTime = System.nanoTime();

            // Check whether the NFC is available.
            sNfcController.isNfcAvailable();

            long endTime = System.nanoTime();
            times.add(endTime - startTime);
        }
        Double asDouble = times.stream().mapToLong(Long::longValue).average().getAsDouble();
        verify.expectSmaller(changeNSToUS(asDouble), 1);
    }

    private Double changeNSToUS(Double asDouble) {
        return asDouble / 1000;
    }
}
