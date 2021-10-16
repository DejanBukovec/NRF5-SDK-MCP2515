var group__nrf__libuarte__drv =
[
    [ "libUARTE library configuration", "group__nrf__libuarte__drv__config.html", "group__nrf__libuarte__drv__config" ],
    [ "nrf_libuarte_drv_data_t", "structnrf__libuarte__drv__data__t.html", [
      [ "length", "structnrf__libuarte__drv__data__t.html#a51d15fda9694a1ec44233bf8cccfa35e", null ],
      [ "p_data", "structnrf__libuarte__drv__data__t.html#ac1985d6d1fd7500eeb2217a8b6316fae", null ]
    ] ],
    [ "nrf_libuarte_drv_overrun_err_evt_t", "structnrf__libuarte__drv__overrun__err__evt__t.html", [
      [ "overrun_length", "structnrf__libuarte__drv__overrun__err__evt__t.html#a9adbafa0a159066ab1d1629edd4c3835", null ]
    ] ],
    [ "nrf_libuarte_drv_evt_t", "structnrf__libuarte__drv__evt__t.html", [
      [ "data", "structnrf__libuarte__drv__evt__t.html#ade56c048333d4b66e077efb4045715b1", null ],
      [ "errorsrc", "structnrf__libuarte__drv__evt__t.html#a480f65b62070152fbd9ad41a82a1ae7b", null ],
      [ "overrun_err", "structnrf__libuarte__drv__evt__t.html#a5e060a6b88ab599bd5bd129fe7ad4037", null ],
      [ "rxtx", "structnrf__libuarte__drv__evt__t.html#a528825d3ef25cc5c088ad16e367bcd5f", null ],
      [ "type", "structnrf__libuarte__drv__evt__t.html#a66a598903829e7b55a45f46428783ae0", null ]
    ] ],
    [ "nrf_libuarte_drv_config_t", "structnrf__libuarte__drv__config__t.html", [
      [ "baudrate", "structnrf__libuarte__drv__config__t.html#a89a9655a60bee420316b05f3f110345c", null ],
      [ "cts_pin", "structnrf__libuarte__drv__config__t.html#a3c2f58d44bcd8ab0da4c16dd03f06338", null ],
      [ "endrx_evt", "structnrf__libuarte__drv__config__t.html#a439f17e2ac957703579839993a66c773", null ],
      [ "hwfc", "structnrf__libuarte__drv__config__t.html#a713f2d3b5a7302c321924a55e14727b0", null ],
      [ "irq_priority", "structnrf__libuarte__drv__config__t.html#acd63a054623520b34bb0dacdfe21361d", null ],
      [ "parity", "structnrf__libuarte__drv__config__t.html#a32068357c83d24bb1963caac98af4681", null ],
      [ "pullup_rx", "structnrf__libuarte__drv__config__t.html#a2dfc83f103877b0181fe470a14c4fe2e", null ],
      [ "rts_pin", "structnrf__libuarte__drv__config__t.html#a57882ce7805cedc9e9f77ab5f25c7044", null ],
      [ "rx_pin", "structnrf__libuarte__drv__config__t.html#a005a06d4680cbe8d47a5c0b555df0144", null ],
      [ "rxdone_tsk", "structnrf__libuarte__drv__config__t.html#a0806509393a75df91a44bf05939cf68e", null ],
      [ "rxstarted_tsk", "structnrf__libuarte__drv__config__t.html#a76bf9ec024330d4e2fb79eb0288412d7", null ],
      [ "startrx_evt", "structnrf__libuarte__drv__config__t.html#a5570729e5ffff550d619c824dd5c8b7e", null ],
      [ "tx_pin", "structnrf__libuarte__drv__config__t.html#a203324d55591352da1eaa2209e8de503", null ]
    ] ],
    [ "nrf_libuarte_drv_ctrl_blk_t", "structnrf__libuarte__drv__ctrl__blk__t.html", [
      [ "chunk_size", "structnrf__libuarte__drv__ctrl__blk__t.html#ad355545efd6fb1988a6aa0833f02ca87", null ],
      [ "context", "structnrf__libuarte__drv__ctrl__blk__t.html#a2d7aa1742ff17d7fcf2fc171e472bd5c", null ],
      [ "enabled", "structnrf__libuarte__drv__ctrl__blk__t.html#a35e373a7fc165c701d16bbdaa39fdb2f", null ],
      [ "evt_handler", "structnrf__libuarte__drv__ctrl__blk__t.html#ac727bcdfbb2f73563fe783f7b6a1b7e1", null ],
      [ "last_pin_rx_byte_cnt", "structnrf__libuarte__drv__ctrl__blk__t.html#adaa2e3e821f605609c26e450b3a550e2", null ],
      [ "last_rx_byte_cnt", "structnrf__libuarte__drv__ctrl__blk__t.html#adee9a1d35e5dd8104d19f5b3e574971b", null ],
      [ "p_cur_rx", "structnrf__libuarte__drv__ctrl__blk__t.html#a99a44375a29486de65c9268626c67018", null ],
      [ "p_next_next_rx", "structnrf__libuarte__drv__ctrl__blk__t.html#a3fcff582c2473eeb8652444c646136ed", null ],
      [ "p_next_rx", "structnrf__libuarte__drv__ctrl__blk__t.html#a2cbc87a9438b292adec5a6e5536c68a0", null ],
      [ "p_tx", "structnrf__libuarte__drv__ctrl__blk__t.html#a0e38e7e57d429e86b86b5630ec65001f", null ],
      [ "ppi_channels", "structnrf__libuarte__drv__ctrl__blk__t.html#ab07cac17619db004e175b489d85640b9", null ],
      [ "ppi_groups", "structnrf__libuarte__drv__ctrl__blk__t.html#a099a0efe7afdef7dfe0b952a9253ba40", null ],
      [ "rts_manual", "structnrf__libuarte__drv__ctrl__blk__t.html#aceff5910c13a9b01988fa159a7a71f37", null ],
      [ "rts_pin", "structnrf__libuarte__drv__ctrl__blk__t.html#ad6fc4852c2fabb2b3d2ff007dadca97f", null ],
      [ "tx_chunk8", "structnrf__libuarte__drv__ctrl__blk__t.html#a7df20fb103a3f939e55b247e49ae315b", null ],
      [ "tx_cur_idx", "structnrf__libuarte__drv__ctrl__blk__t.html#ace8c182c3f690ede95193495d3d4b805", null ],
      [ "tx_len", "structnrf__libuarte__drv__ctrl__blk__t.html#acc1ec7ad9aa58dac03d57d2e7f3a02e5", null ]
    ] ],
    [ "nrf_libuarte_drv_t", "structnrf__libuarte__drv__t.html", [
      [ "ctrl_blk", "structnrf__libuarte__drv__t.html#a967990d2e59cf366e4aa91a0bceb55f1", null ],
      [ "timer", "structnrf__libuarte__drv__t.html#a40447f5f076f1153606465f706d4a212", null ],
      [ "uarte", "structnrf__libuarte__drv__t.html#a2fe331a5e5d6532723f72728bca72c8b", null ]
    ] ],
    [ "nrf_libuarte_drv_evt_type_t", "group__nrf__libuarte__drv.html#ga61f0a544410573972acecc4ad7149267", [
      [ "NRF_LIBUARTE_DRV_EVT_RX_DATA", "group__nrf__libuarte__drv.html#gga61f0a544410573972acecc4ad7149267a3f42a03a6e595a71dd9e848b641bf669", null ],
      [ "NRF_LIBUARTE_DRV_EVT_RX_BUF_REQ", "group__nrf__libuarte__drv.html#gga61f0a544410573972acecc4ad7149267a4348c48657e8a4f6b9340eacfd7183c4", null ],
      [ "NRF_LIBUARTE_DRV_EVT_TX_DONE", "group__nrf__libuarte__drv.html#gga61f0a544410573972acecc4ad7149267a0d3d2b2e91c4fcbfa05cd89ee6cc918a", null ],
      [ "NRF_LIBUARTE_DRV_EVT_ERROR", "group__nrf__libuarte__drv.html#gga61f0a544410573972acecc4ad7149267a26975fc6aa37c0c3a6d82f4a9a3d667a", null ],
      [ "NRF_LIBUARTE_DRV_EVT_OVERRUN_ERROR", "group__nrf__libuarte__drv.html#gga61f0a544410573972acecc4ad7149267a31112bcf195935fe75ca2413af07a037", null ]
    ] ],
    [ "nrf_libuarte_drv_ppi_channel_t", "group__nrf__libuarte__drv.html#ga9256e0e7d0ad250c9fcfe137a7106272", null ],
    [ "nrf_libuarte_drv_ppi_group_t", "group__nrf__libuarte__drv.html#ga42d7994724d4d2d88fad3da42acb598f", [
      [ "NRF_LIBUARTE_DRV_PPI_GROUP_ENDRX_STARTRX", "group__nrf__libuarte__drv.html#gga42d7994724d4d2d88fad3da42acb598fad39ab42b9ceed111cb53be9d2475c9a9", null ],
      [ "NRF_LIBUARTE_DRV_PPI_GROUP_ENDRX_EXT_RXDONE_TSK", "group__nrf__libuarte__drv.html#gga42d7994724d4d2d88fad3da42acb598fa940dfe5a8cd3518ea3e2d19a8517fe21", null ]
    ] ],
    [ "nrf_libuarte_drv_init", "group__nrf__libuarte__drv.html#ga2dcae976b01a3fc7d8644ee509e48135", null ],
    [ "nrf_libuarte_drv_rts_clear", "group__nrf__libuarte__drv.html#gaa95f85ea0bc48a7b7987d2bcd1beaa5a", null ],
    [ "nrf_libuarte_drv_rts_set", "group__nrf__libuarte__drv.html#ga6b1b62a55e9267a47a9fc1ee90be1523", null ],
    [ "nrf_libuarte_drv_rx_buf_rsp", "group__nrf__libuarte__drv.html#gacfebff304c12f5db722d8deb33a0486a", null ],
    [ "nrf_libuarte_drv_rx_start", "group__nrf__libuarte__drv.html#ga27ba26d182cc5318c2199124ece52667", null ],
    [ "nrf_libuarte_drv_rx_stop", "group__nrf__libuarte__drv.html#ga6822cb786794ba9cc620bbd2821f27ce", null ],
    [ "nrf_libuarte_drv_tx", "group__nrf__libuarte__drv.html#gae94fdfe170409e3067f7b052de5a52cf", null ],
    [ "nrf_libuarte_drv_uninit", "group__nrf__libuarte__drv.html#ga4dc4bcb0aff3f89932b586a70fd5cc29", null ]
];