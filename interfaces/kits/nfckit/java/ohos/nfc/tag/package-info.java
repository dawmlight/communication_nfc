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

/**
 * Reads and writes NFC tags.
 *
 * <p>{@link TagInfo}: creates tags and obtains tag information.
 * <p>{@link TagManager}: controls tag read and write. Classes for different types of tags inherit
 * from this abstract class to control connections to tags, read data from tags, and write data to tags.
 * <p>{@link NfcATag}: inherits from the {@link TagManager} abstract class to control connections to
 * NFC-A tags, read data from NFC-A tags, and write data to NFC-A tags.
 * <p>{@link IsoDepTag}: inherits from the {@link TagManager} abstract class to control connections
 * to ISO-DEP tags, read data from ISO-DEP tags, and write data to ISO-DEP tags
 * <p>{@link MifareClassicTag}: inherits from the {@link TagManager} abstract class to control
 * connections to Mifare Classic tags, and read data from and write data to Mifare Classic tags.
 * <p>{@link NdefTag}: inherits from the {@link TagManager} abstract class to control connections to
 * NDEF tags, and read data from and write data to NDEF tags.
 * <p>{@link NdefMessage}: represents the NDEF message.
 * <p>{@link MessageRecord}: represents the NDEF message record.
 * <p>{@link MifareUltralightTag}: inherits from the {@link TagManager} abstract class to control connections to
 * MIFARE Ultralight tags, and read data from and write data to MIFARE Ultralight tags.
 * <p>{@link NfcBTag}: inherits from the {@link TagManager} abstract class to control connections to NFC-B tags, and
 * read data from and write data to NFC-B tags.
 *
 * @since 1
 */

package ohos.nfc.tag;
