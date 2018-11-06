static int dissect_nhdr_extopt(tvbuff_t * tvb, int offset, packet_info * pinfo, proto_tree * tree, lbmc_extopt_reassembled_data_t * reassembly)
{
    proto_item * subtree_item = NULL;
    proto_tree * subtree = NULL;
    guint8 hdrlen = 0;
    static const int * flags[] =
    {
        &hf_lbmc_extopt_flags_ignore,
        &hf_lbmc_extopt_flags_ignore_subtype,
        &hf_lbmc_extopt_flags_more_fragments,
        NULL
    };
    proto_item * ritem = NULL;
    proto_tree * rtree = NULL;
    guint8 flags_val = 0;
    int len_dissected = 0;
    int data_len = 0;
    guint16 subtype;
    guint16 fragment_offset;
    int data_offset;

    hdrlen = tvb_get_guint8(tvb, offset + O_LBMC_EXTOPT_HDR_T_HDR_LEN);
    flags_val = tvb_get_guint8(tvb, offset + O_LBMC_EXTOPT_HDR_T_FLAGS);
    subtype = tvb_get_ntohs(tvb, offset + O_LBMC_EXTOPT_HDR_T_SUBTYPE);
    fragment_offset = tvb_get_ntohs(tvb, offset + O_LBMC_EXTOPT_HDR_T_FRAGMENT_OFFSET);
    subtree_item = proto_tree_add_item(tree, hf_lbmc_extopt, tvb, offset, (gint)hdrlen, ENC_NA);
    subtree = proto_item_add_subtree(subtree_item, ett_lbmc_extopt);
    proto_tree_add_item(subtree, hf_lbmc_extopt_next_hdr, tvb, offset + O_LBMC_EXTOPT_HDR_T_NEXT_HDR, L_LBMC_EXTOPT_HDR_T_NEXT_HDR, ENC_BIG_ENDIAN);
    proto_tree_add_item(subtree, hf_lbmc_extopt_hdr_len, tvb, offset + O_LBMC_EXTOPT_HDR_T_HDR_LEN, L_LBMC_EXTOPT_HDR_T_HDR_LEN, ENC_BIG_ENDIAN);
    proto_tree_add_bitmask(subtree, tvb, offset + O_LBMC_EXTOPT_HDR_T_FLAGS, hf_lbmc_extopt_flags, ett_lbmc_extopt_flags, flags, ENC_BIG_ENDIAN);
    proto_tree_add_item(subtree, hf_lbmc_extopt_id, tvb, offset + O_LBMC_EXTOPT_HDR_T_ID, L_LBMC_EXTOPT_HDR_T_ID, ENC_BIG_ENDIAN);
    proto_tree_add_item(subtree, hf_lbmc_extopt_subtype, tvb, offset + O_LBMC_EXTOPT_HDR_T_SUBTYPE, L_LBMC_EXTOPT_HDR_T_SUBTYPE, ENC_BIG_ENDIAN);
    proto_tree_add_item(subtree, hf_lbmc_extopt_fragment_offset, tvb, offset + O_LBMC_EXTOPT_HDR_T_FRAGMENT_OFFSET, L_LBMC_EXTOPT_HDR_T_FRAGMENT_OFFSET, ENC_BIG_ENDIAN);
    len_dissected = L_LBMC_EXTOPT_HDR_T;
    data_len = (int)hdrlen - len_dissected;
    data_offset = offset + len_dissected;
    len_dissected += data_len;
    if ((flags_val & LBMC_EXTOPT_FLAG_MORE_FRAGMENT) == 0)
    {
        /
        if (reassembly->reassembly_in_progress)
        {
            tvbuff_t * reassembly_tvb;
            gchar * buf;
            proto_item * pi = NULL;

            tvb_memcpy(tvb, reassembly->data + fragment_offset, data_offset, data_len);
            reassembly->len += data_len;
            buf = (gchar *) wmem_memdup(wmem_file_scope(), reassembly->data, reassembly->len);
            reassembly_tvb = tvb_new_real_data(buf, reassembly->len, reassembly->len);
            add_new_data_source(pinfo, reassembly_tvb, "Reassembled EXTOPT fragment data");
            proto_tree_add_item(subtree, hf_lbmc_extopt_data, tvb, data_offset, data_len, ENC_NA);
            ritem = proto_tree_add_item(tree, hf_lbmc_extopt_reassembled_data, reassembly_tvb, 0, reassembly->len, ENC_NA);
            rtree = proto_item_add_subtree(ritem, ett_lbmc_extopt_reassembled_data);
            pi = proto_tree_add_uint(rtree, hf_lbmc_extopt_reassembled_data_subtype, reassembly_tvb, 0, 0, reassembly->subtype);
            PROTO_ITEM_SET_GENERATED(pi);
            pi = proto_tree_add_uint(rtree, hf_lbmc_extopt_reassembled_data_len, reassembly_tvb, 0, 0, (guint32)reassembly->len);
            PROTO_ITEM_SET_GENERATED(pi);
            switch (reassembly->subtype)
            {
                case LBMC_EXT_NHDR_MSGSEL:
                    proto_tree_add_item(rtree, hf_lbmc_extopt_reassembled_data_msgsel, reassembly_tvb, 0, reassembly->len, ENC_ASCII|ENC_NA);
                    break;
                case LBMC_EXT_NHDR_CFGOPT:
                    len_dissected += dissect_nhdr_extopt_cfgopt(reassembly_tvb, 0, pinfo, rtree);
                    break;
                default:
                    proto_tree_add_item(rtree, hf_lbmc_extopt_reassembled_data_data, reassembly_tvb, 0, reassembly->len, ENC_NA);
                    break;
            }
            lbmc_init_extopt_reassembled_data(reassembly);
        }
        else
        {
            switch (subtype)
            {
                case LBMC_EXT_NHDR_MSGSEL:
                    proto_tree_add_item(subtree, hf_lbmc_extopt_msgsel, tvb, data_offset, data_len, ENC_ASCII|ENC_NA);
                    break;
                case LBMC_EXT_NHDR_CFGOPT:
                    len_dissected += dissect_nhdr_extopt_cfgopt(tvb, data_offset, pinfo, subtree);
                    break;
                default:
                    proto_tree_add_item(subtree, hf_lbmc_extopt_data, tvb, data_offset, data_len, ENC_NA);
                    break;
            }
        }
    }
    else
    {
        /
        if (reassembly->reassembly_in_progress)
        {
            tvb_memcpy(tvb, reassembly->data + fragment_offset, data_offset, data_len);
            reassembly->len += data_len;
            proto_tree_add_item(subtree, hf_lbmc_extopt_data, tvb, offset + len_dissected, data_len, ENC_NA);
        }
        else
        {
            reassembly->reassembly_in_progress = TRUE;
            reassembly->subtype = subtype;
            reassembly->len = 0;
            if (fragment_offset != 0)
            {
                expert_add_info_format(pinfo, NULL, &ei_lbmc_analysis_no_reassembly, "LBMC EXTOPT: reassembly not in progress but fragment_offset not zero (%" G_GUINT16_FORMAT ")", fragment_offset);
            }
            else
            {
                tvb_memcpy(tvb, reassembly->data + fragment_offset, data_offset, data_len);
                reassembly->len += data_len;
            }
            proto_tree_add_item(subtree, hf_lbmc_extopt_data, tvb, data_offset, data_len, ENC_NA);
        }
    }
    proto_item_set_len(subtree_item, len_dissected);
    return (len_dissected);
}