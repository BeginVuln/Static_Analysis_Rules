static gint dissect_dmp_security_category (tvbuff_t *tvb, packet_info *pinfo,
                                           proto_tree *tree, GString *label_string,
                                           gint offset, guint8 ext)
{
  proto_tree *field_tree = NULL;
  proto_item *tf = NULL, *tr = NULL;
  gchar      *sec_cat = NULL;
  guint8      message;
  gboolean    country_code = FALSE;

  message = tvb_get_guint8 (tvb, offset);
  tf = proto_tree_add_uint_format (tree, hf_message_sec_cat_nat, tvb,
                                   offset, 1, message, "Security Categories");
  field_tree = proto_item_add_subtree (tf, ett_message_sec_cat);

  switch (ext) {
    
  case SEC_CAT_EXT_NONE:
    proto_tree_add_item (field_tree, hf_message_sec_cat_cl, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item (field_tree, hf_message_sec_cat_cs, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item (field_tree, hf_message_sec_cat_ex, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item (field_tree, hf_message_sec_cat_ne, tvb, offset, 1, ENC_BIG_ENDIAN);
    
    tr = proto_tree_add_item (field_tree, hf_reserved_0x08, tvb, offset, 1, ENC_BIG_ENDIAN);
    if (message & 0x08) {
      expert_add_info_format (pinfo, tr, PI_UNDECODED, PI_WARN, "Reserved value");
    }
    tr = proto_tree_add_item (field_tree, hf_reserved_0x04, tvb, offset, 1, ENC_BIG_ENDIAN);
    if (message & 0x04) {
      expert_add_info_format (pinfo, tr, PI_UNDECODED, PI_WARN, "Reserved value");
    }
    
    if (message & 0xF0) {
      sec_cat = ep_strdup_printf ("%s%s%s%s",
                                  (message & 0x80) ? ",cl" : "",
                                  (message & 0x40) ? ",cs" : "",
                                  (message & 0x20) ? ",ex" : "",
                                  (message & 0x10) ? ",ne" : "");
      proto_item_append_text (tf, ": %s", &sec_cat[1]);
      g_string_append (label_string, sec_cat);
    }
    break;
    
  case SEC_CAT_EXT_PERMISSIVE:
    if ((message >> 2) == 0x3F) {
      /
      proto_tree_add_uint_format (field_tree, hf_message_sec_cat_permissive, tvb, offset, 1,
                                  message, "1111 11.. = Next byte has Country Code (0x3F)");
      country_code = TRUE;
    } else {
      tr = proto_tree_add_item (field_tree, hf_message_sec_cat_permissive, tvb, offset, 1, ENC_BIG_ENDIAN);
      proto_item_append_text (tf, ": rel-to-%s", get_nat_pol_id_short (message >> 2));
      g_string_append_printf (label_string, ",rel-to-%s", get_nat_pol_id_short (message >> 2));
      if ((message >> 2) == 0) {
        expert_add_info_format (pinfo, tr, PI_UNDECODED, PI_WARN, "Reserved value");
      }
    }
    break;
    
  case SEC_CAT_EXT_RESTRICTIVE:
    proto_tree_add_item (field_tree, hf_message_sec_cat_restrictive, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_item_append_text (tf, " (restrictive: 0x%2.2x)", message >> 2);
    break;
    
  default:
    break;
  }

  proto_item_append_text (tf, " (0x%2.2x)", message);
  
  if (dmp.version == 1) {
    tr = proto_tree_add_item (field_tree, hf_reserved_0x02, tvb, offset, 1, ENC_BIG_ENDIAN);
    if (message & 0x02) {
      expert_add_info_format (pinfo, tr, PI_UNDECODED, PI_WARN, "Reserved value");
    }
    tr = proto_tree_add_item (field_tree, hf_reserved_0x01, tvb, offset, 1, ENC_BIG_ENDIAN);
    if (message & 0x01) {
      expert_add_info_format (pinfo, tr, PI_UNDECODED, PI_WARN, "Reserved value");
    }
  } else {
    tr = proto_tree_add_item (field_tree, hf_message_sec_cat_extended, tvb, offset, 1, ENC_BIG_ENDIAN);
    if ((message & 0x01) && (message & 0x02)) {
      expert_add_info_format (pinfo, tr, PI_UNDECODED, PI_WARN, "Reserved value");
    } else if (message & 0x01 || message & 0x02) {
      proto_item_append_text (tf, " (extended)");
      offset = dissect_dmp_security_category (tvb, pinfo, tree, label_string, offset+1, message & 0x03);
    }

    if (country_code) {
      proto_tree_add_item (field_tree, hf_message_sec_cat_country_code, tvb, offset+1, 1, ENC_BIG_ENDIAN);
      proto_item_append_text (tf, " (rel-to country-code: %d)", tvb_get_guint8 (tvb, offset+1));
      proto_item_set_len (tf, 2);
      offset++;
    }
  }

  return offset;
}