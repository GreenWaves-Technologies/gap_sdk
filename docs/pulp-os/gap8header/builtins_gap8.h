v2h gap8_pack2(signed short x, signed short y)
{
	__builtin_pulp_pack2((signed short)   (x), (signed short)   (y));
}
v2u gap8_packu2(unsigned short x, unsigned short y)
{
	__builtin_pulp_pack2((unsigned short) (x), (unsigned short) (y));
}
v4s gap8_pack4(signed char x, signed char y, signed char z, signed char t)
{
	__builtin_pulp_pack4((signed char)   (x), (signed char)   (y), (signed char)   (z), (signed char)   (t));
}
v4u gap8_packu4(unsigned char x, unsigned char y, unsigned char z, unsigned char t)
{
	__builtin_pulp_pack4((unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t));
}
v2s gap8_max2(v2s x, v2s y)
{
	__builtin_pulp_max2((x), (y));
}
v4s gap8_max4(v4s x, v4s y)
{
	__builtin_pulp_max4((x), (y));
}
v2u gap8_maxu2(v2s x, v2s y)
{
	__builtin_pulp_maxu2((x), (y));
}
v4u gap8_maxu4(v4s x, v4s y)
{
	__builtin_pulp_maxu4((x), (y));
}
v2s gap8_min2(v2s x, v2s y)
{
	__builtin_pulp_min2((x), (y));
}
v4s gap8_min4(v4s x, v4s y)
{
	__builtin_pulp_min4((x), (y));
}
v2u gap8_minu2(v2s x, v2s y)
{
	__builtin_pulp_minu2((x), (y));
}
v4u gap8_minu4(v4s x, v4s y)
{
	__builtin_pulp_minu4((x), (y));
}
int gap8_clip(int x, int precision)
{
	__builtin_pulp_clip((x), -(1<<(precision)), (1<<precision)-1);
}
int gap8_clip_r(int x, int bound)
{
	__builtin_pulp_clip_r((x), (bound));
}
unsigned int gap8_clipu(int x, int precision)
{
	__builtin_pulp_clipu((x), 0, (1<<precision)-1);
}
unsigned int gap8_clipu_r(int x, int bound)
{
	__builtin_pulp_clipu_r((x), (bound));
}
v2u gap8_abs2(v2s x)
{
	__builtin_pulp_abs2((x));
}
v4u gap8_abs4(v4s x)
{
	__builtin_pulp_abs4((x));
}
v2s gap8_neg2(v2s x)
{
	__builtin_pulp_neg2((x));
}
v4s gap8_neg4(v4s x)
{
	__builtin_pulp_neg4((x));
}
v2s gap8_add2(v2s x, v2s y)
{
	__builtin_pulp_add2((x), (y));
}
v4s gap8_add4(v4s x, v4s y)
{
	__builtin_pulp_add4((x), (y));
}
v2s gap8_sub2(v2s x, v2s y)
{
	__builtin_pulp_sub2((x), (y));
}
v4s gap8_sub4(v4s x, v4s y)
{
	__builtin_pulp_sub4((x), (y));
}
v2s gap8_avg2(v2s x, v2s y)
{
	__builtin_pulp_avg2((x), (y));
}
v4s gap8_avg4(v4s x, v4s y)
{
	__builtin_pulp_avg4((x), (y));
}
v2u gap8_avgu2(v2s x, v2s y)
{
	__builtin_pulp_avgu2((x), (y));
}
v4u gap8_avgu4(v4s x, v4s y)
{
	__builtin_pulp_avgu4((x), (y));
}
v2s gap8_and2(v2s x, v2s y)
{
	__builtin_pulp_and2((x), (y));
}
v4s gap8_and4(v4s x, v4s y)
{
	__builtin_pulp_and4((x), (y));
}
v2s gap8_or2(v2s x, v2s y)
{
	__builtin_pulp_or2((x), (y));
}
v4s gap8_or4(v4s x, v4s y)
{
	__builtin_pulp_or4((x), (y));
}
v2s gap8_exor2(v2s x, v2s y)
{
	__builtin_pulp_exor2(x, y);
}
v4s gap8_exor4(v4s x, v4s y)
{
	__builtin_pulp_exor4(x, y);
}
v2u gap8_srl2(v2s x, v2s y)
{
	__builtin_pulp_srl2(x, y);
}
v4u gap8_srl4(v4s x, v4s y)
{
	__builtin_pulp_srl4(x, y);
}
v2s gap8_sra2(v2s x, v2s y)
{
	__builtin_pulp_sra2(x, y);
}
v4s gap8_sra4(v4s x, v4s y)
{
	__builtin_pulp_sra4(x, y);
}
v2s gap8_sll2(v2s x, v2s y)
{
	__builtin_pulp_sll2(x, y);
}
v4s gap8_sll4(v4s x, v4s y)
{
	__builtin_pulp_sll4(x, y);
}
int gap8_mac(int Acc, int x, int y)
{
	__builtin_pulp_mac((x), (y), (Acc));
}
int gap8_msu(int Acc, int x, int y)
{
	__builtin_pulp_msu((x), (y), (Acc));
}
int gap8_macs(int Acc, int x, int y)
{
	__builtin_pulp_macs((x), (y), (Acc));
}
int gap8_machhs(int Acc, int x, int y)
{
	__builtin_pulp_machhs((x), (y), (Acc));
}
unsigned int gap8_macu(unsigned int Acc, int x, int y)
{
	__builtin_pulp_macu((x), (y), (Acc));
}
unsigned int gap8_machhu(unsigned int Acc, int x, int y)
{
	__builtin_pulp_machhu((x), (y), (Acc));
}
int gap8_macsN(int Acc, int x, int y, int n)
{
	__builtin_pulp_macsN((x), (y), (Acc), (n));
}
unsigned int gap8_macuN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_macuN((x), (y), (Acc), (n));
}
int gap8_macsRN(int Acc, int x, int y, int n)
{
	__builtin_pulp_macsRN((x), (y), (Acc), (n), (1<<((n)-1)));
}
unsigned int gap8_macuRN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_macuRN((x), (y), (Acc), (n), (1<<((n)-1)));
}
int gap8_machhsN(int Acc, int x, int y, int n)
{
	__builtin_pulp_machhsN((x), (y), (Acc), (n));
}
unsigned int gap8_machhuN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_machhuN((x), (y), (Acc), (n));
}
int gap8_machhsRN(int Acc, int x, int y, int n)
{
	__builtin_pulp_machhsN((x), (y), (Acc), (n), (1<<((n)-1)));
}
unsigned int gap8_machhuRN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_machhuN((x), (y), (Acc), (n), (1<<((n)-1)));
}
int gap8_muls(int x, int y)
{
	__builtin_pulp_muls((x), (y));
}
unsigned int gap8_mulu(int x, int y)
{
	__builtin_pulp_mulu((x), (y));
}
int gap8_mulhhs(int x, int y)
{
	__builtin_pulp_mulhhs((x), (y));
}
unsigned int gap8_mulhhu(int x, int y)
{
	__builtin_pulp_mulhhu((x), (y));
}
int gap8_mulsN(int x, int y, int n)
{
	__builtin_pulp_mulsN((x), (y), (n));
}
int gap8_mulsRN(int x, int y, int n)
{
	__builtin_pulp_mulsRN((x), (y), (n), (1<<((n)-1)));
}
unsigned int gap8_muluN(int x, int y, int n)
{
	__builtin_pulp_muluN((x), (y), (n));
}
unsigned int gap8_muluRN(int x, int y, int n)
{
	__builtin_pulp_muluRN((x), (y), (n), (1<<((n)-1)));
}
int gap8_mulhhsN(int x, int y, int n)
{
	__builtin_pulp_mulhhsN((x), (y), (n));
}
int gap8_mulhhsRN(int x, int y, int n)
{
	__builtin_pulp_mulhhsRN((x), (y), (n), (1<<((n)-1)));
}
unsigned int gap8_mulhhuN(int x, int y, int n)
{
	__builtin_pulp_mulhhuN((x), (y), (n));
}
unsigned int gap8_mulhhuRN(int x, int y, int n)
{
	__builtin_pulp_mulhhuRN((x), (y), (n), (1<<((n)-1)));
}
int gap8_dotp2(v2s x, v2s y)
{
	__builtin_pulp_dotsp2((x), (y));
}
unsigned int gap8_dotpu2(v2s x, v2s y)
{
	__builtin_pulp_dotup2((x), (y));
}
int gap8_dotpus2(v2s x, v2s y)
{
	__builtin_pulp_dotusp2((x), (y));
}
int gap8_dotpsc2(v2s x, int y)
{
	__builtin_pulp_dotspsc2((x), (y));
}
unsigned int gap8_dotpusc2(v2s x, int y)
{
	__builtin_pulp_dotupsc2((x), (y));
}
int gap8_dotpussc2(v2s x, int y)
{
	__builtin_pulp_dotuspsc2((x), (y));
}
int gap8_sumdotp2(v2s x, v2s y, int z)
{
	__builtin_pulp_sdotsp2((x), (y), (z));
}
unsigned int gap8_sumdotpu2(v2s x, v2s y, unsigned int z)
{
	__builtin_pulp_sdotup2((x), (y), (z));
}
int gap8_sumdotpus2(v2s x, v2s y, int z)
{
	__builtin_pulp_sdotusp2((x), (y), (z));
}
int gap8_sumdotpsc2(v2s x, int y, int z)
{
	__builtin_pulp_sdotspsc2((x), (y), (z));
}
unsigned int gap8_sumdotpusc2(v2s x, int y, unsigned int z)
{
	__builtin_pulp_sdotupsc2((x), (y), (z));
}
int gap8_sumdotpussc2(v2s x, int y, int z)
{
	__builtin_pulp_sdotuspsc2((x), (y), (z));
}
int gap8_dotp4(v4s x, v4s y)
{
	__builtin_pulp_dotsp4((x), (y));
}
unsigned int gap8_dotpu4(v4s x, v4s y)
{
	__builtin_pulp_dotup4((x), (y));
}
int gap8_dotpus4(v4s x, v4s y)
{
	__builtin_pulp_dotusp4((x), (y));
}
int gap8_dotpsc4(v4s x, int y)
{
	__builtin_pulp_dotspsc4((x), (y));
}
unsigned int gap8_dotpusc4(v4s x, int y)
{
	__builtin_pulp_dotupsc4((x), (y));
}
int gap8_dotpussc4(v4s x, int y)
{
	__builtin_pulp_dotuspsc4((x), (y));
}
int gap8_sumdotp4(v4s x, v4s y, int z)
{
	__builtin_pulp_sdotsp4((x), (y), (z));
}
unsigned int gap8_sumdotpu4(v4s x, v4s y, unsigned int z)
{
	__builtin_pulp_sdotup4((x), (y), (z));
}
int gap8_sumdotpus4(v4s x, v4s y, int z)
{
	__builtin_pulp_sdotusp4((x), (y), (z));
}
int gap8_sumdotpsc4(v4s x, int y, int z)
{
	__builtin_pulp_sdotspsc4((x), (y), (z));
}
unsigned int gap8_sumdotpusc4(v4s x, int y, unsigned int z)
{
	__builtin_pulp_sdotupsc4((x), (y), (z));
}
int gap8_sumdotpussc4(v4s x, int y, int z)
{
	__builtin_pulp_sdotuspsc4((x), (y), (z));
}
v2s gap8_cplxmuls(v2s x, v2s y)
{
	__builtin_pulp_cplxmuls((x), (y));
}
v2s gap8_cplxmulsdiv2(v2s x, v2s y)
{
	__builtin_pulp_cplxmulsdiv2((x), (y));
}
v2s gap8_cplxmulsdiv4(v2s x, v2s y)
{
	__builtin_pulp_cplxmulsdiv4((x), (y));
}
v2s gap8_cplxconj(v2s x)
{
	__builtin_pulp_cplx_conj((x));
}
v2s gap8_sub2rotmj(v2s x, v2s y)
{
	__builtin_pulp_sub2rotmj((x), (y));
}
v2s gap8_add2div2(v2s x, v2s y)
{
	__builtin_pulp_add2div2((x), (y));
}
v2s gap8_add2div4(v2s x, v2s y)
{
	__builtin_pulp_add2div4((x), (y));
}
v2s gap8_add4div2(v2s x, v2s y)
{
	__builtin_pulp_add4div2((x), (y));
}
v2s gap8_add4div4(v2s x, v2s y)
{
	__builtin_pulp_add4div4((x), (y));
}
v2s gap8_sub2div2(v2s x, v2s y)
{
	__builtin_pulp_sub2div2((x), (y));
}
v2s gap8_sub2div4(v2s x, v2s y)
{
	__builtin_pulp_sub2div4((x), (y));
}
v2s gap8_sub4div2(v2s x, v2s y)
{
	__builtin_pulp_sub4div2((x), (y));
}
v2s gap8_sub4div4(v2s x, v2s y)
{
	__builtin_pulp_sub4div4((x), (y));
}
v2s gap8_vitmax(v2s x, v2s y)
{
	__builtin_pulp_vitmax2((x), (y));
}
v2s gap8_vitsel(v2s x, v2s y)
{
	__builtin_pulp_vitsel2((x), (y));
}
int gap8_fl1(int x)
{
	__builtin_pulp_fl1((x));
}
int gap8_clb(int x)
{
	__builtin_pulp_clb((x));
}
int gap8_bitset(int x, int size, int off)
{
	__builtin_pulp_bset((x), (((1<<(size))-1)<<(off)));
}
int gap8_bitset_r(int x, int size, int off)
{
	__builtin_pulp_bset_r((x), ExtInsMaskFast((size), (off)));
}
int gap8_bitset_r_safe(int x, int size, int off)
{
	__builtin_pulp_bset_r((x), ExtInsMaskSafe((size), (off)));
}
int gap8_bitclr(int x, int size, int off)
{
	__builtin_pulp_bclr((x), ~(((1<<(size))-1)<<(off)));
}
int gap8_bitclr_r(int x, int size, int off)
{
	__builtin_pulp_bclr_r((x), ExtInsMaskFast((size), (off)));
}
int gap8_bitclr_r_safe(int x, int size, int off)
{
	__builtin_pulp_bclr_r((x), ExtInsMaskSafe((size), (off)));
}
int gap8_bitextract(int x, int size, int off)
{
	__builtin_pulp_bextract((x), (size), (off));
}
unsigned int gap8_bitextractu(int x, int size, int off)
{
	__builtin_pulp_bextractu((x), (size), (off));
}
int gap8_bitextract_r(int x, int size, int off)
{
	__builtin_pulp_bextract_r((x), ExtInsMaskFast((size), (off)));
}
unsigned int gap8_bitextractu_r(int x, int size, int off)
{
	__builtin_pulp_bextractu_r((x), ExtInsMaskFast((size), (off)));
}
int gap8_bitextract_r_safe(int x, int size, int off)
{
	__builtin_pulp_bextract_r((x), ExtInsMaskSafe((size), (off)));
}
unsigned int gap8_bitextractu_r_safe(int x, int size, int off)
{
	__builtin_pulp_bextractu_r((x), ExtInsMaskSafe((size), (off)));
}
int gap8_bitinsert(int dst, int src, int size, int off)
{
	__builtin_pulp_binsert((dst), ~(((1<<(size))-1)<<(off)), (src), (((1<<(size))-1)<<(off)), (off));
}
int gap8_bitinsert_r(int dst, int src, int size, int off)
{
	__builtin_pulp_binsert_r((dst), (src), ExtInsMaskFast((size), (off)));
}
int gap8_bitinsert_r_safe(int dst, int src, int size, int off)
{
	__builtin_pulp_binsert_r((dst), (src), ExtInsMaskSafe((size), (off)));
}
int gap8_rotr(int x)
{
	__builtin_pulp_rotr((x));
}
unsigned int gap8_addnormu(int x, int y, int scale)
{
	__builtin_pulp_adduN((x), (y), (scale));
}
unsigned int gap8_addnormu_reg(int x, int y, int scale)
{
	__builtin_pulp_adduN_r((x), (y), (scale));
}
int gap8_addnorm(int x, int y, int scale)
{
	__builtin_pulp_addN((x), (y), (scale));
}
int gap8_addnorm_reg(int x, int y, int scale)
{
	__builtin_pulp_addN_r((x), (y), (scale));
}
unsigned int gap8_addroundnormu(int x, int y, int scale)
{
	__builtin_pulp_adduRN((x), (y), (scale), (1<<((scale)-1)));
}
unsigned int gap8_addroundnormu_reg(int x, int y, int scale)
{
	__builtin_pulp_adduRN_r((x), (y), (scale));
}
int gap8_addroundnorm(int x, int y, int scale)
{
	__builtin_pulp_addRN((x), (y), (scale), (1<<((scale)-1)));
}
int gap8_addroundnorm_reg(int x, int y, int scale)
{
	__builtin_pulp_addRN_r((x), (y), (scale));
}
unsigned int gap8_subnormu(int x, int y, int scale)
{
	__builtin_pulp_subuN((x), (y), (scale));
}
unsigned int gap8_subnormu_reg(int x, int y, int scale)
{
	__builtin_pulp_subuN_r((x), (y), (scale));
}
int gap8_subnorm(int x, int y, int scale)
{
	__builtin_pulp_subN((x), (y), (scale));
}
int gap8_subnorm_reg(int x, int y, int scale)
{
	__builtin_pulp_subN_r((x), (y), (scale));
}
unsigned int gap8_subroundnormu(int x, int y, int scale)
{
	__builtin_pulp_subuRN((x), (y), (scale), (1<<((scale)-1)));
}
unsigned int gap8_subroundnormu_reg(int x, int y, int scale)
{
	__builtin_pulp_subuRN_r((x), (y), (scale));
}
int gap8_subroundnorm(int x, int y, int scale)
{
	__builtin_pulp_subRN((x), (y), (scale), (1<<((scale)-1)));
}
int gap8_subroundnorm_reg(int x, int y, int scale)
{
	__builtin_pulp_subRN_r((x), (y), (scale));
}
unsigned int gap8_roundnormu(int x, int scale)
{
	__builtin_pulp_adduRN((x), 0, (scale), (1<<((scale)-1)));
}
int gap8_roundnormu_reg(int x, int scale)
{
	__builtin_pulp_adduRN_r((x), 0, (scale));
}
unsigned int gap8_roundnorm(int x, int scale)
{
	__builtin_pulp_addRN((x), 0, (scale), (1<<((scale)-1)));
}
int gap8_roundnorm_reg(int x, int scale)
{
	__builtin_pulp_addRN_r((x), 0, (scale));
}
int gap8_coreid()
{
	__builtin_pulp_CoreId();
}
int gap8_clusterid()
{
	__builtin_pulp_ClusterId();
}
int gap8_ncore()
{
	__builtin_pulp_CoreCount();
}
int gap8_isfc()
{
	__builtin_pulp_IsFc();
}
void gap8_sprwrite(int x, int y)
{
	__builtin_pulp_spr_write(x, y);
}
int gap8_sprread(int x)
{
	__builtin_pulp_spr_read(x);
}
int gap8_sprread_vol(int x)
{
	__builtin_pulp_spr_read_vol(x);
}
void gap8_sprbitset(int spr, int off)
{
	__builtin_pulp_spr_bit_set((spr), (off));
}
void gap8_sprbitclr(int spr, int off)
{
	__builtin_pulp_spr_bit_clr((spr), (off));
}
int gap8_sprreadthenwrite(int spr, int x)
{
	__builtin_pulp_read_then_spr_write((spr), (x));
}
int gap8_sprreadthenbitset(int spr, int off)
{
	__builtin_pulp_read_then_spr_bit_set((spr), (off));
}
int gap8_sprreadthenbitclr(int spr, int off)
{
	__builtin_pulp_read_then_spr_bit_clr((spr), (off));
}
int gap8_read_base_off(int * base, int off)
{
	__builtin_pulp_read_base_off((base), (off));
}
void gap8_write_base_off(int * base, int off, int val)
{
	__builtin_pulp_write_base_off((base), (off), (val));
}
int gap8_read_base_off_vol(int * base, int off)
{
	__builtin_pulp_OffsetedRead((base), (off));
}
short int gap8_read_base_off_half_vol(short int * base, int off)
{
	__builtin_pulp_OffsetedReadHalf((base), (off));
}
signed char gap8_read_base_off_byte_vol(signed char * base, int off)
{
	__builtin_pulp_OffsetedReadByte((base), (off));
}
void gap8_write_base_off_vol(int x, int * base, int off)
{
	__builtin_pulp_OffsetedWrite((x), (base), (off));
}
void gap8_write_base_off_half_vol(int x, short int * base, int off)
{
	__builtin_pulp_OffsetedWriteHalf((x), (base), (off));
}
void gap8_write_base_off_byte_vol(int x, signed char * base, int off)
{
	__builtin_pulp_OffsetedWriteByte((x), (base), (off));
}
