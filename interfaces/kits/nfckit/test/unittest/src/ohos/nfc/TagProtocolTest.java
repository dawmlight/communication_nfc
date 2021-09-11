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

import ohos.nfc.tag.NfcATag;
import ohos.nfc.tag.NfcBTag;
import ohos.nfc.tag.NfcFTag;
import ohos.nfc.tag.NfcVTag;
import ohos.nfc.tag.MifareClassicTag;
import ohos.nfc.tag.MifareUltralightTag;
import ohos.nfc.tag.IsoDepTag;
import ohos.nfc.tag.ResponseInfo;
import ohos.nfc.tag.TagInfo;
import ohos.nfc.tag.TagManager;
import ohos.nfc.tag.MessageRecord;
import ohos.nfc.tag.NdefMessage;
import ohos.nfc.tag.NdefTag;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.interwork.utils.PacMapEx;
import ohos.utils.Parcel;
import ohos.utils.Sequenceable;
import ohos.unittest.CaseLevel;
import ohos.unittest.CaseType;
import ohos.unittest.Level;
import ohos.unittest.Type;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import java.io.IOException;
import java.util.Optional;

/**
 * implement test cases for tag
 *
 * @author yanghan
 */
public class TagProtocolTest {
    private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, NfcKitsUtils.NFC_DOMAIN_ID,
        "TagProtocolTest");
    private static final int WAIT_TIME = 3 * 1000;

    private NfcATag mNfcATag = null;
    private MifareClassicTag mMifareClassicTag = null;
    private NdefTag mNdefTag = null;
    private TagInfo mTagInfo = null;
    private NfcBTag mNfcBTag = null;
    private MifareUltralightTag mMifareUltralightTag = null;

    /** set up environment. */
    @BeforeClass
    public static void beforeClass() throws Exception {
        /**
         * @tc.setup: open NFC
         */
        boolean isSuccess = NfcController.getInstance(new ohos.aafwk.ability.Ability()).openNfc();
        Assert.assertEquals(isSuccess, true);
        Thread.sleep(WAIT_TIME);
    }

    /** tear down environment. */
    @AfterClass
    public static void afterClass() throws Exception {
    }

    /** set up every test case condition. */
    @Before
    public void setUp() throws Exception {
        /**
         * @tc.setup: create the instance of NFC
         */
        HiLog.info(LABEL, "TagProtocolTest setUp()");
        PacMapEx extraNfcATag = new PacMapEx();
        extraNfcATag.putObjectValue("sak", (short) 0x08);
        extraNfcATag.putObjectValue("atqa", new byte[]{(byte) 0xB0, (byte) 0x00});
        PacMapEx extraIso = new PacMapEx();
        extraIso.putObjectValue("hiresp", new byte[]{(byte) 0x90, (byte) 0x00});
        extraIso.putObjectValue("histbytes", new byte[]{(byte) 0xA0, (byte) 0x00});
        PacMapEx extraMifare = new PacMapEx();
        extraMifare.putObjectValue("sak", (short) 0x08);
        extraMifare.putObjectValue("atqa", new byte[]{(byte) 0xB0, (byte) 0x00});
        PacMapEx extraNdef = new PacMapEx();
        extraNdef.putObjectValue("ndefmaxlength", 10);
        extraNdef.putObjectValue("ndefcardstate", 1);
        MessageRecord[] records = new MessageRecord[1];
        records[0] = new MessageRecord((byte) 0x00, null, null, null);
        extraNdef.putObjectValue("ndefmsg", new NdefMessage(records));
        extraNdef.putObjectValue("ndeftype", 1);
        PacMapEx extraNfcBTag = new PacMapEx();
        extraNfcBTag.putObjectValue("appdata", new byte[]{(byte) 0xA0, (byte) 0xC0});
        extraNfcBTag.putObjectValue("protinfo", new byte[]{(byte) 0x13, (byte) 0x1F});
        PacMapEx extraNfcFTag = new PacMapEx();
        extraNfcFTag.putObjectValue("systemcode", new byte[]{(byte) 0xA0, (byte) 0xC0});
        extraNfcFTag.putObjectValue("pmm", new byte[]{(byte) 0x13, (byte) 0x1F});
        PacMapEx extraNfcVTag = new PacMapEx();
        extraNfcVTag.putObjectValue("respflags", (byte) 0xA0);
        extraNfcVTag.putObjectValue("dsfid", (byte) 0x13);
        PacMapEx extraMifareUltra = new PacMapEx();
        extraMifareUltra.putObjectValue("isulc", false);
        PacMapEx[] extras = {extraNfcATag, extraIso, extraMifare, extraNdef, extraNfcBTag, extraMifareUltra,
            extraNfcFTag, extraNfcVTag};
        byte[] id = {0x15, 0x27, 0x39, 0x52};
        int[] profiles = {TagManager.NFC_A, TagManager.ISO_DEP, TagManager.MIFARE_CLASSIC, TagManager.NDEF,
            TagManager.NFC_B, TagManager.MIFARE_ULTRALIGHT, TagManager.NFC_F, TagManager.NFC_V};
        mTagInfo = new TagInfo(id, profiles, extras, 0);
        mNfcATag = NfcATag.getInstance(mTagInfo);
        mMifareClassicTag = MifareClassicTag.getInstance(mTagInfo);
        Optional<NdefTag> nfcNdefTagOpt = NdefTag.getInstance(mTagInfo);
        mNdefTag = nfcNdefTagOpt.get();
        mNfcBTag = NfcBTag.getInstance(mTagInfo);
        mMifareUltralightTag = MifareUltralightTag.getInstance(mTagInfo);
    }

    /** tear down every test case condition. */
    @After
    public void tearDown() throws Exception {
        /**
         * @tc.teardown: destroy the instance of NFC
         */
        mNfcATag = null;
        mMifareClassicTag = null;
        mNdefTag = null;
        mTagInfo = null;
        mNfcBTag = null;
        mMifareUltralightTag = null;
        Thread.sleep(WAIT_TIME);
    }

    /**
     * @tc.name: testTagProtocol001
     * @tc.desc: test class TagInfo
     * @tc.require: AR000CSJ7F AR000CSJ6B AR000CSJ7B SR000CSJ3R
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol001() {
        /**
         * @tc.steps step1 TagInfo instance
         * @tc.expected step1 the elements of TagInfo exist
         */
        TagInfo tagInfo = mNfcATag.getTagInfo();
        Assert.assertNotNull(tagInfo.getTagId());
        Assert.assertNotNull(tagInfo.getTagSupportedProfiles());
        Assert.assertNotNull(tagInfo.getProfileExtras(TagManager.NFC_A));
        Assert.assertTrue(tagInfo.isProfileSupported(TagManager.NFC_A));
    }

    /**
     * @tc.name: testTagProtocol002
     * @tc.desc: connect tag
     * @tc.require: AR000CSJ7F SR000CSJ3R
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol002() {
        /**
         * @tc.steps step1 connect tag
         * @tc.expected step1 tag is not connected
         */
        Assert.assertFalse(mNfcATag.connectTag());
        Assert.assertFalse(mNfcATag.isTagConnected());
    }

    /**
     * @tc.name: testTagProtocol003
     * @tc.desc: reconnect tag
     * @tc.require: AR000CSJ7F SR000CSJ3R
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol003() {
        /**
         * @tc.steps step1 reconnect tag
         * @tc.expected step1 tag is not connected
         */
        mNfcATag.reset();
        Assert.assertFalse(mNfcATag.isTagConnected());
    }

    /**
     * @tc.name: testTagProtocol004
     * @tc.desc: set sendDataTimeout of tag
     * @tc.require: AR000CSJ7F SR000CSJ3R
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol004() {
        /**
         * @tc.steps step1 set data timeout and then get
         * @tc.expected step1 the get timeout equals set timeout
         */
        int timeout = 1000;
        boolean isSuccess = mNfcATag.setSendDataTimeout(timeout);
        Assert.assertTrue(isSuccess);
        int getData = mNfcATag.getSendDataTimeout();
        Assert.assertEquals(timeout, getData);

        /**
         * @tc.steps step1 reset data timeout
         * @tc.expected step1 the get timeout is greater than zero
         */
        mNfcATag.reset();
        getData = mNfcATag.getSendDataTimeout();
        Assert.assertTrue(getData > 0);
    }

    /**
     * @tc.name: testTagProtocol005
     * @tc.desc: send data
     * @tc.require: AR000CSJ7F SR000CSJ6F
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol005() {
        /**
         * @tc.steps step1 get max send length
         * @tc.expected step1 the length is greater than zero
         */
        int length = mNfcATag.getMaxSendLength();
        Assert.assertTrue(length > 0);

        byte[] data = {0x13, 0x59, 0x22};
        byte[] resp = mNfcATag.sendData(data);
    }

    /**
     * @tc.name: testTagProtocol006
     * @tc.desc: get Iso14443A tag data
     * @tc.require: AR000CSJ7F SR000CSJ6F
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol006() {
        /**
         * @tc.steps step1 get sak and atqa
         * @tc.expected step1 the result is success
         */
        Assert.assertNotEquals(0, mNfcATag.getSak());
        Assert.assertNotNull(mNfcATag.getAtqa());
    }

    /**
     * @tc.name: testTagProtocol007
     * @tc.desc: get instance of IsoDepTag
     * @tc.require: AR000CSJ6B SR000CSJ6F
     * @tc.author: yanghan
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol007() {
        /**
         * @tc.steps step1 get instance of IsoDepTag
         * @tc.expected step1 the instance is not null
         */
        Optional<IsoDepTag> nfIsoDepTagOpt = IsoDepTag.getInstance(mTagInfo);
        boolean isPresent = nfIsoDepTagOpt.isPresent();
        Assert.assertTrue(isPresent);
        IsoDepTag tag = nfIsoDepTagOpt.get();
        tag.getHiLayerResponse();
        tag.getHistoricalBytes();

        ResponseInfo responseInfo = new ResponseInfo();
        responseInfo.getResponse();
        Parcel parcelOut = Parcel.create();
        Assert.assertTrue(responseInfo.marshalling(parcelOut));
        Parcel parcelIn = Parcel.create();
        parcelIn.writeInt(1);
        parcelIn.writeInt(1);
        parcelIn.writeByteArray(new byte[0]);
        Assert.assertTrue(responseInfo.unmarshalling(parcelIn));

        // test marshalling and unmarshalling of TagInfo
        byte[] id = {0x15, 0x27};
        int[] profiles = {TagManager.NFC_A};
        PacMapEx extraNfcATag = new PacMapEx();
        extraNfcATag.putObjectValue("sak", (short) 0x08);
        extraNfcATag.putObjectValue("atqa", new byte[]{(byte) 0xB0, (byte) 0x00});
        PacMapEx[] extras = {extraNfcATag};
        TagInfo tagInfo = new TagInfo(id, profiles, extras, 1);
        parcelOut = Parcel.create();
        tagInfo.marshalling(parcelOut);
        Parcel parcelIn2 = Parcel.create();
        parcelIn2.writeByteArray(id);
        parcelIn2.writeIntArray(profiles);
        parcelIn2.writeSequenceableArray((Sequenceable[]) extras);
        parcelIn2.writeInt(1);
    }

    /**
     * @tc.name: testTagProtocol008
     * @tc.desc: get instance of MifareClassicTag
     * @tc.require: AR000DOUOO SR000DOUOP
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol008() {
        /**
         * @tc.steps step1 get instance of MifareClassicTag
         * @tc.expected step1 the instance is not null
         */
        MifareClassicTag mifareClassicTag = MifareClassicTag.getInstance(mTagInfo);
        Assert.assertNotNull(mifareClassicTag);
        mifareClassicTag.getMifareType();
        mifareClassicTag.getTagSize();
        mifareClassicTag.getSectorsNum();
        mifareClassicTag.getBlocksNum();
        mifareClassicTag.getBlocksNumForSector(1);
        mifareClassicTag.getSectorId(1);
        mifareClassicTag.getFirstBlockId(1);

        mifareClassicTag.connectTag();
        // The example key for NFC authenticate the Mifare Classic sector.
        byte[] KEY_NFC_FORUM =
            {(byte) 0xD3, (byte) 0xF7, (byte) 0xD3, (byte) 0xF7, (byte) 0xD3, (byte) 0xF7};
        try {
            mifareClassicTag.authenSectorUseKey(1, KEY_NFC_FORUM, (byte)0x60);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when authenSectorUseKey");
        }

        // The example value that will write into the block.
        byte[] value = {0x15, 0x27, 0x15,  0x27, 0x15, 0x27, 0x15, 0x27,
            0x15, 0x27, 0x15,  0x27, 0x15, 0x27, 0x15, 0x27};
        try {
            // 1 means write to the first block
            mifareClassicTag.writeBlock(1, value);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when writeBlock");
        }
        try {
            // 1 means read from the first block
            byte[] readValue = mifareClassicTag.readBlock(1);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when readBlock");
        }
        try {
            // 1, 2 means increase value 2 to block 1
            mifareClassicTag.incBlock(1, 2);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when incBlock");
        }
        try {
            // 1, 2 means decrease value 2 to block 1
            mifareClassicTag.decBlock(1, 2);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when decBlock");
        }
        try {
            // 1 means restore the first block
            mifareClassicTag.restoreBlock(1);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when restoreBlock");
        }
    }

    /**
     * @tc.name: testTagProtocol009
     * @tc.desc: get instance of NdefTag
     * @tc.require: AR000DOUON SR000DOUOP
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol009() {
        NfcErrCodes.isError(0);

        Assert.assertNotNull(mNdefTag);
        mNdefTag.getNdefMessage();
        mNdefTag.getNdefMaxSize();
        mNdefTag.getTagType();

        mNdefTag.connectTag();
        try {
            mNdefTag.readNdefMessage();
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when readNdefMessage");
        } catch (IOException e) {
            HiLog.error(LABEL, "IOException when readNdefMessage");
        }

        MessageRecord[] records = new MessageRecord[1];
        records[0] = new MessageRecord((byte) 0x00, null, null, null);
        try {
            mNdefTag.writeNdefMessage(new NdefMessage(records));
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when writeNdefMessage");
        } catch (IOException e) {
            HiLog.error(LABEL, "IOException when writeNdefMessage");
        }

        mNdefTag.canSetReadOnly();
        try {
            mNdefTag.setReadOnly();
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when setReadOnly");
        } catch (IOException e) {
            HiLog.error(LABEL, "IOException when setReadOnly");
        }
        mNdefTag.isNdefWritable();
    }

    /**
     * @tc.name: testTagProtocol010
     * @tc.desc: get instance of NdefTag
     * @tc.require: AR000DOUON SR000DOUOP
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol010() {
        MessageRecord messageRecord1 = new MessageRecord((byte) 0x00, null, null, null);
        MessageRecord messageRecord2 = new MessageRecord((byte) 0x01, null, null, null);
        Assert.assertNotNull(messageRecord1);
        Assert.assertNotNull(messageRecord2);
        messageRecord1.buildUri();
        messageRecord1.getPayload();
        messageRecord1.getId();
        messageRecord1.getType();
        messageRecord1.getTnf();
        messageRecord1.equals(messageRecord2);
        messageRecord1.hashCode();
        messageRecord1.marshalling(Parcel.create());
        messageRecord1.unmarshalling(Parcel.create());
        MessageRecord.buildApplicationRecord("com.ohos.nfc");
        MessageRecord.buildMime("com/ohos", new byte[]{(byte) 0xB0, (byte) 0x00});
        MessageRecord.buildExternal("ohos", "A", new byte[]{(byte) 0xB0, (byte) 0x00});
        MessageRecord.buildTextRecord("a", "A");
        MessageRecord[] records = new MessageRecord[1];
        records[0] = new MessageRecord((byte) 0x00, null, null, null);
        NdefMessage ndefMessage = new NdefMessage(records);
        ndefMessage.getRecords();
        ndefMessage.getAllRecordsLength();
        ndefMessage.hashCode();
        ndefMessage.equals(ndefMessage);
        ndefMessage.marshalling(Parcel.create());
        ndefMessage.unmarshalling(Parcel.create());
    }

    /**
     * @tc.name: testTagProtocol011
     * @tc.desc: get instance of NfcBTag
     * @tc.require: AR000DOUOQ SR000DOUOP
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol011() {
        /**
         * @tc.steps step1 get instance of NfcBTag
         * @tc.expected step1 the instance is not null
         */
        NfcBTag nfcBTag = NfcBTag.getInstance(mTagInfo);
        Assert.assertNotNull(nfcBTag);
        nfcBTag.getRespAppData();
        nfcBTag.getRespProtocol();
    }

    /**
     * @tc.name: testTagProtocol012
     * @tc.desc: get instance of MifareUltralightTag
     * @tc.require: AR000DOUN7 SR000DOUOP
     * @tc.author: maolongxia
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol012() {
        /**
         * @tc.steps step1 get instance of MifareUltralightTag
         * @tc.expected step1 the instance is not null
         */
        MifareUltralightTag mifareUltralightTag = MifareUltralightTag.getInstance(mTagInfo);
        Assert.assertNotNull(mifareUltralightTag);

        mifareUltralightTag.getMifareType();
        mifareUltralightTag.connectTag();
        try {
            mifareUltralightTag.readFourPages(0);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when readFourPages");
        } catch (IOException e) {
            HiLog.error(LABEL, "IOException when readFourPages");
        }

        byte[] data = new byte[]{(byte)0x01, (byte)0x02, (byte)0x03, (byte)0x04};
        try {
            mifareUltralightTag.writeOnePage(0, data);
        } catch (IllegalStateException e) {
            HiLog.error(LABEL, "IllegalStateException when writeOnePage");
        } catch (IOException e) {
            HiLog.error(LABEL, "IOException when writeOnePage");
        }
    }

    /**
     * @tc.name: testTagProtocol013
     * @tc.desc: get instance of NfcFTag
     * @tc.require: AR000F4G5A
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol013() {
        /**
         * @tc.steps step1 get instance of NfcFTag
         * @tc.expected step1 the instance is not null
         */
        Optional<NfcFTag> nfcFTagOpt = NfcFTag.getInstance(mTagInfo);
        boolean isPresent = nfcFTagOpt.isPresent();
        Assert.assertTrue(isPresent);
        NfcFTag nfcFTag = nfcFTagOpt.get();
        byte[] systemCode = nfcFTag.getSystemCode();
        Assert.assertEquals((byte) 0xA0, systemCode[0]);
        Assert.assertEquals((byte) 0xC0, systemCode[1]);
        byte[] pmm = nfcFTag.getPMm();
        Assert.assertEquals((byte) 0x13, pmm[0]);
        Assert.assertEquals((byte) 0x1F, pmm[1]);
    }

    /**
     * @tc.name: testTagProtocol014
     * @tc.desc: get instance of NfcVTag
     * @tc.require: AR000F4G5A
     * @tc.author: liujiawei
     */
    @Test
    @CaseLevel(level = Level.LEVEL_1)
    @CaseType(type = Type.FUNC)
    public void testTagProtocol014() {
        /**
         * @tc.steps step1 get instance of NfcVTag
         * @tc.expected step1 the instance is not null
         */
        Optional<NfcVTag> nfcVTagOpt = NfcVTag.getInstance(mTagInfo);
        boolean isPresent = nfcVTagOpt.isPresent();
        Assert.assertTrue(isPresent);
        NfcVTag nfcVTag = nfcVTagOpt.get();
        byte responseFlags = nfcVTag.getResponseFlags();
        Assert.assertEquals((byte) 0xA0, responseFlags);
        byte dsfId = nfcVTag.getDsfId();
        Assert.assertEquals((byte) 0x13, dsfId);
    }
}
