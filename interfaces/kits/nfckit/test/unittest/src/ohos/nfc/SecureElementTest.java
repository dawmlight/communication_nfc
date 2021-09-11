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
import ohos.nfc.oma.Aid;
import ohos.nfc.oma.Channel;
import ohos.nfc.oma.Session;
import ohos.nfc.oma.Reader;
import ohos.nfc.oma.SEService;
import ohos.nfc.oma.SecureElementCallbackProxy;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.util.Optional;

/**
 * implement test cases for SEService
 *
 * @author zhangxiuping
 */
public class SecureElementTest {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "SecureElementTest");

    /** testcase execution result */
    private static final int EXEC_FAILED = -1;
    private static final int EXEC_SUCCESS = 0;
    private static final int WAIT_TIME = 3 * 1000;
    private static int execResult = EXEC_SUCCESS;
    private SEService mSEService = null;

    private class TestServiceConnectedCallback implements SEService.OnCallback {
        @Override
        public void serviceConnected() {
            HiLog.info(LABEL, "TestServiceConnectedCallback serviceConnected()");
        }
    }

    private boolean isEse(String readerName) {
        return ("eSE1".equals(readerName) || "eSE".equals(readerName));
    }

    /**
     * set up every test case condition
     *
     * @throws Exception setup failed
     */
    @Before
    public void setUp() throws Exception {
        // setup one time.
        if (mSEService == null) {
            HiLog.info(LABEL, "testSecureElement000 setUp()");
            // need a Context object for SEService if necessary.
            mSEService = new SEService(new ohos.aafwk.ability.Ability(), new TestServiceConnectedCallback());
        }
        execResult = EXEC_SUCCESS;
    }

    /**
     * tear down every test case condition
     *
     * @throws Exception tearDown failed
     */
    @After
    public void tearDown() throws Exception {
    }

    /**
     * @tc.name: testSecureElement001
     * @tc.desc: create SEService and bind SA
     * @tc.require: AR000CSJO9 SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement001() {
        // SEService
        Assert.assertNotNull(mSEService);
    }

    /**
     * @tc.name: testSecureElement002
     * @tc.desc: check isConnected
     * @tc.require: AR000CSJO9 SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement002() {
        // SEService
        try {
            Assert.assertNotNull(mSEService);
            Thread.sleep(WAIT_TIME);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }

        // isConnected
        boolean isConnected = mSEService.isConnected();
        Assert.assertTrue(isConnected);
    }

    /**
     * @tc.name: testSecureElement003
     * @tc.desc: check getReaders
     * @tc.require: AR000CSJO9 SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement003() {
        // SEService
        try {
            Assert.assertNotNull(mSEService);
            Thread.sleep(WAIT_TIME);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }

        // isConnected
        boolean isConnected = mSEService.isConnected();
        Assert.assertTrue(isConnected);

        // getReaders
        Reader[] readers = mSEService.getReaders();
        Assert.assertNotNull(readers);
        HiLog.info(LABEL, "testSecureElement003 readers length = %{public}d", readers.length);
        Assert.assertTrue(readers.length > 0);
    }

    /**
     * @tc.name: testSecureElement004
     * @tc.desc: check the version
     * @tc.require: AR000CSJO9 SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement004() {
        // SEService
        Assert.assertNotNull(mSEService);

        // getVersion
        String version = mSEService.getVersion();
        HiLog.info(LABEL, "testSecureElement004 OMA version = %{public}s", version);
        Assert.assertEquals(version, "1.0");
    }

    /**
     * @tc.name: testSecureElement005
     * @tc.desc: check getReaderName
     * @tc.require: AR000CSJP7 SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement005() {
        // SEService
        try {
            Assert.assertNotNull(mSEService);
            Thread.sleep(WAIT_TIME);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }

        // isConnected
        boolean isConnected = mSEService.isConnected();
        Assert.assertTrue(isConnected);

        // getReaders
        Reader[] readers = mSEService.getReaders();
        Assert.assertNotNull(readers);
        Assert.assertTrue(readers.length > 0);

        // getReaderName
        boolean hasESE = false;
        for (int i = 0; i < readers.length; i++) {
            if (isEse(readers[i].getName())) {
                hasESE = true;
                break;
            }
        }
        Assert.assertEquals(hasESE, true);
    }

    /**
     * @tc.name: testSecureElement006
     * @tc.desc: check openBasicChannel
     * @tc.require: AR000CSJ8C AR000CSJRD SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement006() {
        // SEService
        try {
            Assert.assertNotNull(mSEService);
            Thread.sleep(WAIT_TIME);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }

        // isConnected
        boolean isConnected = mSEService.isConnected();
        Assert.assertTrue(isConnected);

        // getReaders
        Reader[] readers = mSEService.getReaders();
        Assert.assertNotNull(readers);
        Assert.assertTrue(readers.length > 0);

        // getReaderName
        boolean hasESE = false;
        Reader eSe = null;
        for (int i = 0; i < readers.length; i++) {
            if (isEse(readers[i].getName())) {
                hasESE = true;
                eSe = readers[i];
                break;
            }
        }
        Assert.assertTrue(hasESE);

        mSEService.getCallback();
        eSe.getSEService();
        eSe.isSecureElementPresent();

        // open session
        Optional<Session> optionalSession = eSe.openSession();
        Session session = optionalSession.orElse(null);
        Assert.assertNotNull(session);
        HiLog.info(LABEL, "testSecureElement006 openSession completed.");

        byte[] atr = session.getATR();
        Assert.assertNotNull(atr);
        HiLog.info(LABEL, "testSecureElement006 getATR completed.");

        // openBasicChannel
        if (eSe != null) {
            byte[] aidValue = Aid.hexStringToBytes(Aid.AID_CRS);
            Aid aid = new Aid(aidValue, 0, aidValue.length);

            // check aid validation
            Assert.assertTrue(aid.isAidValid());

            // get aid bytes length
            Assert.assertNotNull(aid.getAidBytes());

            Optional<Channel> optionalChannel = session.openBasicChannel(aid);
            Channel channel = optionalChannel.orElse(null);
            Assert.assertNotNull(channel);
            HiLog.info(LABEL, "testSecureElement006 openBasicChannel completed.");
            Assert.assertTrue(channel.isBasicChannel());

            // get selectResponse
            Assert.assertNotNull(channel.getSelectResponse());
            HiLog.info(LABEL, "testSecureElement006 getSelectResponse completed.");

            // closeChannel
            Assert.assertTrue(!channel.isClosed());
            channel.closeChannel();
            Assert.assertTrue(channel.isClosed());
        }

        // close session
        session.close();
        eSe.closeSessions();

        // close SE
        mSEService.shutdown();
    }

    /**
     * @tc.name: testSecureElement007
     * @tc.desc: check openLogicalChannel
     * @tc.require: AR000CSJ8C AR000CSJRD SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement007() {
        // SEService
        try {
            Assert.assertNotNull(mSEService);
            Thread.sleep(WAIT_TIME);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }

        // isConnected
        boolean isConnected = mSEService.isConnected();
        Assert.assertTrue(isConnected);

        // getReaders
        Reader[] readers = mSEService.getReaders();
        Assert.assertNotNull(readers);
        Assert.assertTrue(readers.length > 0);

        // getReaderName
        boolean hasESE = false;
        Reader eSe = null;
        for (int i = 0; i < readers.length; i++) {
            if (isEse(readers[i].getName())) {
                hasESE = true;
                eSe = readers[i];
                break;
            }
        }
        Assert.assertEquals(hasESE, true);

        // open session
        Optional<Session> optionalSession = eSe.openSession();
        Session session = optionalSession.orElse(null);
        Assert.assertNotNull(session);
        HiLog.info(LABEL, "testSecureElement007 openSession completed.");

        // openLogicalChannel
        if (eSe != null) {
            byte[] aidValue = Aid.hexStringToBytes(Aid.AID_CRS);
            Aid aid = new Aid(aidValue, 0, aidValue.length);

            Optional<Channel> optionalChannel = session.openLogicalChannel(aid);
            Channel channel = optionalChannel.orElse(null);
            Assert.assertNotNull(channel);
            HiLog.info(LABEL, "testSecureElement007 openLogicalChannel completed.");
            Assert.assertTrue(!channel.isBasicChannel());

            // closeChannel
            Assert.assertTrue(!channel.isClosed());
            channel.closeChannel();
            Assert.assertTrue(channel.isClosed());
        }

        // close session
        session.close();
    }

    /**
     * @tc.name: testSecureElement008
     * @tc.desc: check transmit
     * @tc.require: AR000CSJP5 SR000CSJ84
     * @tc.author: zhangxiuping
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testSecureElement008() {
        // SEService
        try {
            Assert.assertNotNull(mSEService);
            Thread.sleep(WAIT_TIME);
        } catch (InterruptedException interruptedException) {
            execResult = EXEC_FAILED;
        }

        // isConnected
        boolean isConnected = mSEService.isConnected();
        Assert.assertTrue(isConnected);

        // getReaders
        Reader[] readers = mSEService.getReaders();
        Assert.assertNotNull(readers);
        Assert.assertTrue(readers.length > 0);

        // getReaderName
        boolean hasESE = false;
        Reader eSe = null;
        for (int i = 0; i < readers.length; i++) {
            if (isEse(readers[i].getName())) {
                hasESE = true;
                eSe = readers[i];
                break;
            }
        }
        Assert.assertEquals(hasESE, true);

        // open session
        Optional<Session> optionalSession = eSe.openSession();
        Session session = optionalSession.orElse(null);
        Assert.assertNotNull(session);
        HiLog.info(LABEL, "testSecureElement008 openSession completed.");

        // openLogicalChannel
        if (eSe != null) {
            byte[] aidValue = Aid.hexStringToBytes(Aid.AID_CRS);
            Aid aid = new Aid(aidValue, 0, aidValue.length);
            Optional<Channel> optionalChannel = session.openLogicalChannel(aid);
            Channel channel = optionalChannel.orElse(null);
            Assert.assertNotNull(channel);
            Assert.assertTrue(!channel.isBasicChannel());

            byte[] resp = channel.transmit(Aid.hexStringToBytes("00a40000020000"));
            Assert.assertNotNull(resp);
            Assert.assertTrue(resp.length > 0);
            HiLog.info(LABEL, "testSecureElement008 resp = %{public}s", Aid.byteArrayToHexString(resp));

            // closeChannel
            try {
                Thread.sleep(WAIT_TIME);
            } catch (InterruptedException interruptedException) {
                execResult = EXEC_FAILED;
            }
            channel.closeChannel();
            Assert.assertTrue(channel.isClosed());

            // close session
            session.close();

            mSEService.notifyServiceConnected(true);
            SecureElementCallbackProxy mCallbackProxy = mSEService.getCallbackProxy();
            mCallbackProxy.onServiceConnected();
            mCallbackProxy.onServiceDisconnected();
            // close objects connected to the service
            mSEService.shutdown();
            Assert.assertTrue(!mSEService.isConnected());
        }
    }
}
