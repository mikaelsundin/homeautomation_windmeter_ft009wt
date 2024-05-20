
TO check if correct settings.



rf.register_Receiver(
	RFMOD_MANCHESTER, // mod
	28012, // initseq
	0, // lo_prefix
	0, // hi_prefix
	0, // first_lo_ign
	488, // lo_short
	978, // lo_long
	0, // hi_short (0 => take lo_short)
	0, // hi_long  (0 => take lo_long)
	972, // lo_last
	38896, // sep
	78  // nb_bits
);

