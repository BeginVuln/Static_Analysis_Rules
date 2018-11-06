static void
dissect_q931_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
    gboolean is_over_ip)
{
    int         offset = 0;
    proto_tree  *q931_tree = NULL;
    proto_tree  *ie_tree = NULL;
    proto_item  *ti;
    guint8      prot_discr;
    guint8      call_ref_len;
    guint8      call_ref[15];
    guint32     call_ref_val;
    guint8      message_type, segmented_message_type;
    guint8      info_element;
    guint16     info_element_len;
    gboolean    first_frag, more_frags;
    guint32     frag_len;
    fragment_head *fd_head;
    tvbuff_t *next_tvb = NULL;

    q931_pi=wmem_new(wmem_packet_scope(), q931_packet_info);

    /
    reset_q931_packet_info(q931_pi);
    have_valid_q931_pi=TRUE;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "Q.931");

    prot_discr = tvb_get_guint8(tvb, offset);
    ti = proto_tree_add_item(tree, proto_q931, tvb, offset, -1, ENC_NA);
    if (tree) {
        q931_tree = proto_item_add_subtree(ti, ett_q931);

        dissect_q931_protocol_discriminator(tvb, offset, q931_tree);
    }
    offset += 1;
    call_ref_len = tvb_get_guint8(tvb, offset) & 0xF;   /
    if (q931_tree != NULL)
        proto_tree_add_uint(q931_tree, hf_q931_call_ref_len, tvb, offset, 1, call_ref_len);
    offset += 1;
    switch (call_ref_len) {
        case 0: call_ref_val = 0; break;
        case 1: call_ref_val = tvb_get_guint8(tvb, offset); break;
        case 2: call_ref_val = tvb_get_ntohs(tvb, offset); break;
        case 3: call_ref_val = tvb_get_ntoh24(tvb, offset); break;
        default: call_ref_val = tvb_get_ntohl(tvb, offset);
    }
    if (call_ref_len != 0) {
        tvb_memcpy(tvb, call_ref, offset, call_ref_len);
        if (q931_tree != NULL) {
            proto_tree_add_boolean(q931_tree, hf_q931_call_ref_flag,
                tvb, offset, 1, (call_ref[0] & 0x80) != 0);
            call_ref[0] &= 0x7F;
            proto_tree_add_bytes(q931_tree, hf_q931_call_ref,
                tvb, offset, call_ref_len, call_ref);
        } else
        {       /
            call_ref[0] &= 0x7F;
        }
        /
        memcpy(&(q931_pi->crv), call_ref, call_ref_len > sizeof(q931_pi->crv) ? sizeof(q931_pi->crv) : call_ref_len );
        offset += call_ref_len;
    }
    message_type = tvb_get_guint8(tvb, offset);
    if(have_valid_q931_pi && q931_pi) {
        q931_pi->message_type = message_type;
    }
    col_add_str(pinfo->cinfo, COL_INFO, get_message_name(prot_discr, message_type));

    if (prot_discr == NLPID_DMS)
        proto_tree_add_item(q931_tree, hf_q931_maintenance_message_type, tvb, offset, 1, ENC_BIG_ENDIAN);
    else
        proto_tree_add_item(q931_tree, hf_q931_message_type, tvb, offset, 1, ENC_BIG_ENDIAN);

    offset += 1;

    /
    if ((message_type != Q931_SEGMENT) || !q931_reassembly ||
            (tvb_reported_length_remaining(tvb, offset) <= 4)) {
        dissect_q931_IEs(tvb, pinfo, tree, q931_tree, is_over_ip, offset, 0);
        return;
    }
    info_element = tvb_get_guint8(tvb, offset);
    info_element_len = tvb_get_guint8(tvb, offset + 1);
    if ((info_element != Q931_IE_SEGMENTED_MESSAGE) || (info_element_len < 2)) {
        dissect_q931_IEs(tvb, pinfo, tree, q931_tree, is_over_ip, offset, 0);
        return;
    }
    /
    ie_tree = proto_tree_add_subtree(q931_tree, tvb, offset, 1+1+info_element_len, ett_q931_ie[info_element], NULL,
                    val_to_str(info_element, q931_info_element_vals[0], "Unknown information element (0x%02X)"));
    proto_tree_add_uint_format_value(ie_tree, hf_q931_information_element, tvb, offset, 1, info_element,
                            "%s", val_to_str(info_element, q931_info_element_vals[0], "Unknown (0x%02X)"));
    proto_tree_add_item(ie_tree, hf_q931_information_element_len, tvb, offset + 1, 1, ENC_NA);
    dissect_q931_segmented_message_ie(tvb, pinfo, offset + 2, info_element_len, ie_tree, ti);
    first_frag = (tvb_get_guint8(tvb, offset + 2) & 0x80) != 0;
    more_frags = (tvb_get_guint8(tvb, offset + 2) & 0x7F) != 0;
    segmented_message_type = tvb_get_guint8(tvb, offset + 3);
    col_append_fstr(pinfo->cinfo, COL_INFO, " of %s",
            val_to_str_ext(segmented_message_type, &q931_message_type_vals_ext, "Unknown message type (0x%02X)"));

    offset += 1 + 1 + info_element_len;
    /
    frag_len = tvb_reported_length_remaining(tvb, offset);
    if (first_frag && fragment_get(&q931_reassembly_table, pinfo, call_ref_val, NULL)) {
        /
        fragment_end_seq_next(&q931_reassembly_table, pinfo, call_ref_val, NULL);
    }
    fd_head = fragment_add_seq_next(&q931_reassembly_table,
                    tvb, offset, pinfo, call_ref_val, NULL,
                    frag_len, more_frags);
    if (fd_head) {
        if (pinfo->num == fd_head->reassembled_in) {  /
            if (fd_head->next != NULL) {  /
                next_tvb = tvb_new_chain(tvb, fd_head->tvb_data);
                add_new_data_source(pinfo, next_tvb, "Reassembled Q.931 IEs");
                /
                if (tree) {
                    proto_item *frag_tree_item;
                    show_fragment_seq_tree(fd_head, &q931_frag_items, q931_tree, pinfo, next_tvb, &frag_tree_item);
                }
            } else {  /
                next_tvb = tvb_new_subset_remaining(tvb, offset);
            }

            col_add_fstr(pinfo->cinfo, COL_INFO, "%s [reassembled]",
                    val_to_str_ext(segmented_message_type, &q931_message_type_vals_ext, "Unknown message type (0x%02X)"));

        } else {
            if (tree) proto_tree_add_uint(q931_tree, hf_q931_reassembled_in, tvb, offset, frag_len, fd_head->reassembled_in);
        }
    }
    if (next_tvb)
        dissect_q931_IEs(next_tvb, pinfo, tree, q931_tree, is_over_ip, 0, 0);
}