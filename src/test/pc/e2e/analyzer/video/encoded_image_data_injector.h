/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_PC_E2E_ANALYZER_VIDEO_ENCODED_IMAGE_DATA_INJECTOR_H_
#define TEST_PC_E2E_ANALYZER_VIDEO_ENCODED_IMAGE_DATA_INJECTOR_H_

#include <cstdint>
#include <utility>

#include "api/video/encoded_image.h"

namespace webrtc {
namespace webrtc_pc_e2e {

// Injects frame id into EncodedImage on encoder side
class EncodedImageDataInjector {
 public:
  virtual ~EncodedImageDataInjector() = default;

  // Return encoded image with specified |id| and |discard| flag injected into
  // its payload. |discard| flag mean does analyzing decoder should discard this
  // encoded image because it belongs to unnecessary simulcast stream or spatial
  // layer. |coding_entity_id| is unique id of decoder or encoder.
  virtual EncodedImage InjectData(uint16_t id,
                                  bool discard,
                                  const EncodedImage& source,
                                  int coding_entity_id) = 0;
};

struct EncodedImageExtractionResult {
  uint16_t id;
  EncodedImage image;
  // Is true if encoded image should be discarded. It is used to filter out
  // unnecessary spatial layers and simulcast streams.
  bool discard;
};

// Extracts frame id from EncodedImage on decoder side.
class EncodedImageDataExtractor {
 public:
  virtual ~EncodedImageDataExtractor() = default;

  // Returns encoded image id, extracted from payload and also encoded image
  // with its original payload. For concatenated spatial layers it should be the
  // same id. |coding_entity_id| is unique id of decoder or encoder.
  virtual EncodedImageExtractionResult ExtractData(const EncodedImage& source,
                                                   int coding_entity_id) = 0;
};

}  // namespace webrtc_pc_e2e
}  // namespace webrtc

#endif  // TEST_PC_E2E_ANALYZER_VIDEO_ENCODED_IMAGE_DATA_INJECTOR_H_
