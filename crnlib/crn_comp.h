// File: crn_comp.h
// See Copyright Notice and license at the end of inc/crnlib.h
#pragma once

#include "../inc/crn_defs.h"

#include "../inc/crnlib.h"
#include "crn_symbol_codec.h"
#include "crn_dxt_hc.h"
#include "crn_image.h"
#include "crn_image_utils.h"
#include "crn_texture_comp.h"

namespace crnlib {
class crn_comp : public itexture_comp {
  CRNLIB_NO_COPY_OR_ASSIGNMENT_OP(crn_comp);

 public:
  crn_comp();
  virtual ~crn_comp();

  virtual const char* get_ext() const { return "CRN"; }

  virtual bool compress_init(const crn_comp_params& params);
  virtual bool compress_pass(const crn_comp_params& params, float* pEffective_bitrate);
  virtual void compress_deinit();

  virtual const crnlib::vector<uint8>& get_comp_data() const { return m_comp_data; }
  virtual crnlib::vector<uint8>& get_comp_data() { return m_comp_data; }

  uint get_comp_data_size() const { return m_comp_data.size(); }
  const uint8* get_comp_data_ptr() const { return m_comp_data.size() ? &m_comp_data[0] : NULL; }

 private:
  task_pool m_task_pool;
  const crn_comp_params* m_pParams;

  image_u8 m_images[cCRNMaxFaces][cCRNMaxLevels];

  enum comp {
    cColor,
    cAlpha0,
    cAlpha1,
    cNumComps
  };

  bool m_has_comp[cNumComps];

  struct level_details {
    uint first_block;
    uint num_blocks;
    uint block_width;
  };
  crnlib::vector<level_details> m_levels;

  uint m_total_blocks;
  crnlib::vector<uint32> m_color_endpoints;
  crnlib::vector<uint32> m_alpha_endpoints;
  crnlib::vector<uint32> m_color_selectors;
  crnlib::vector<uint64> m_alpha_selectors;
  crnlib::vector<dxt_hc::endpoint_indices_details> m_endpoint_indices;
  crnlib::vector<dxt_hc::selector_indices_details> m_selector_indices;

  crnd::crn_header m_crn_header;
  crnlib::vector<uint8> m_comp_data;

  dxt_hc m_hvq;

  symbol_histogram m_reference_hist;
  static_huffman_data_model m_reference_dm;

  symbol_histogram m_endpoint_index_hist[2];
  static_huffman_data_model m_endpoint_index_dm[2];  // color, alpha

  symbol_histogram m_selector_index_hist[2];
  static_huffman_data_model m_selector_index_dm[2];  // color, alpha

  crnlib::vector<uint8> m_packed_blocks[cCRNMaxLevels];
  crnlib::vector<uint8> m_packed_data_models;
  crnlib::vector<uint8> m_packed_color_endpoints;
  crnlib::vector<uint8> m_packed_color_selectors;
  crnlib::vector<uint8> m_packed_alpha_endpoints;
  crnlib::vector<uint8> m_packed_alpha_selectors;

  void clear();

  static float color_endpoint_similarity_func(uint index_a, uint index_b, void* pContext);
  static float alpha_endpoint_similarity_func(uint index_a, uint index_b, void* pContext);
  void sort_color_endpoint_codebook(crnlib::vector<uint>& remapping, const crnlib::vector<uint>& endpoints);
  void sort_alpha_endpoint_codebook(crnlib::vector<uint>& remapping, const crnlib::vector<uint>& endpoints);

  bool pack_color_endpoints(crnlib::vector<uint8>& data, const crnlib::vector<uint>& remapping, uint trial_index);
  bool pack_alpha_endpoints(crnlib::vector<uint8>& data, const crnlib::vector<uint>& remapping, uint trial_index);

  void sort_color_selectors(crnlib::vector<uint>& remapping);
  void sort_alpha_selectors(crnlib::vector<uint>& remapping);

  bool pack_color_selectors(crnlib::vector<uint8>& packed_data, const crnlib::vector<uint>& remapping);
  bool pack_alpha_selectors(crnlib::vector<uint8>& packed_data, const crnlib::vector<uint>& remapping);

  bool alias_images();
  bool quantize_images();

  bool pack_blocks(
      uint group,
      bool clear_histograms,
      symbol_codec* pCodec,
      const crnlib::vector<uint>* pColor_endpoint_remap,
      const crnlib::vector<uint>* pColor_selector_remap,
      const crnlib::vector<uint>* pAlpha_endpoint_remap,
      const crnlib::vector<uint>* pAlpha_selector_remap);

  void optimize_color_endpoint_codebook_task(uint64 data, void* pData_ptr);
  bool optimize_color_endpoint_codebook(crnlib::vector<uint>& remapping);

  bool optimize_color_selector_codebook(crnlib::vector<uint>& remapping);

  void optimize_alpha_endpoint_codebook_task(uint64 data, void* pData_ptr);
  bool optimize_alpha_endpoint_codebook(crnlib::vector<uint>& remapping);

  bool optimize_alpha_selector_codebook(crnlib::vector<uint>& remapping);

  bool create_comp_data();

  bool pack_data_models();

  bool update_progress(uint phase_index, uint subphase_index, uint subphase_total);

  bool compress_internal();

  static void append_vec(crnlib::vector<uint8>& a, const void* p, uint size);
  static void append_vec(crnlib::vector<uint8>& a, const crnlib::vector<uint8>& b);
};

}  // namespace crnlib
