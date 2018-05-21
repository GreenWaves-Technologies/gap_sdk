v2h __PACK2(signed short x, signed short y)
{
	__builtin_pulp_pack2((signed short)   (x), (signed short)   (y));
}
v2u __PACKU2(unsigned short x, unsigned short y)
{
	__builtin_pulp_pack2((unsigned short) (x), (unsigned short) (y));
}
v4s __PACK4(signed char x, signed char y, signed char z, signed char t)
{
	__builtin_pulp_pack4((signed char)   (x), (signed char)   (y), (signed char)   (z), (signed char)   (t));
}
v4u __PACKU4(unsigned char x, unsigned char y, unsigned char z, unsigned char t)
{
	__builtin_pulp_pack4((unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t));
}
v2s __MAX2(v2s x, v2s y)
{
	__builtin_pulp_max2((x), (y));
}
v4s __MAX4(v4s x, v4s y)
{
	__builtin_pulp_max4((x), (y));
}
v2u __MAXU2(v2s x, v2s y)
{
	__builtin_pulp_maxu2((x), (y));
}
v4u __MAXU4(v4s x, v4s y)
{
	__builtin_pulp_maxu4((x), (y));
}
v2s __MIN2(v2s x, v2s y)
{
	__builtin_pulp_min2((x), (y));
}
v4s __MIN4(v4s x, v4s y)
{
	__builtin_pulp_min4((x), (y));
}
v2u __MINU2(v2s x, v2s y)
{
	__builtin_pulp_minu2((x), (y));
}
v4u __MINU4(v4s x, v4s y)
{
	__builtin_pulp_minu4((x), (y));
}
int __CLIP(int x, int precision)
{
	__builtin_pulp_clip((x), -(1<<(precision)), (1<<precision)-1);
}
int __CLIP_R(int x, int bound)
{
	__builtin_pulp_clip_r((x), (bound));
}
unsigned int __CLIPU(int x, int precision)
{
	__builtin_pulp_clipu((x), 0, (1<<precision)-1);
}
unsigned int __CLIPU_R(int x, int bound)
{
	__builtin_pulp_clipu_r((x), (bound));
}
v2u __ABS2(v2s x)
{
	__builtin_pulp_abs2((x));
}
v4u __ABS4(v4s x)
{
	__builtin_pulp_abs4((x));
}
v2s __NEG2(v2s x)
{
	__builtin_pulp_neg2((x));
}
v4s __NEG4(v4s x)
{
	__builtin_pulp_neg4((x));
}
v2s __ADD2(v2s x, v2s y)
{
	__builtin_pulp_add2((x), (y));
}
v4s __ADD4(v4s x, v4s y)
{
	__builtin_pulp_add4((x), (y));
}
v2s __SUB2(v2s x, v2s y)
{
	__builtin_pulp_sub2((x), (y));
}
v4s __SUB4(v4s x, v4s y)
{
	__builtin_pulp_sub4((x), (y));
}
v2s __AVG2(v2s x, v2s y)
{
	__builtin_pulp_avg2((x), (y));
}
v4s __AVG4(v4s x, v4s y)
{
	__builtin_pulp_avg4((x), (y));
}
v2u __AVGU2(v2s x, v2s y)
{
	__builtin_pulp_avgu2((x), (y));
}
v4u __AVGU4(v4s x, v4s y)
{
	__builtin_pulp_avgu4((x), (y));
}
v2s __AND2(v2s x, v2s y)
{
	__builtin_pulp_and2((x), (y));
}
v4s __AND4(v4s x, v4s y)
{
	__builtin_pulp_and4((x), (y));
}
v2s __OR2(v2s x, v2s y)
{
	__builtin_pulp_or2((x), (y));
}
v4s __OR4(v4s x, v4s y)
{
	__builtin_pulp_or4((x), (y));
}
v2s __EXOR2(v2s x, v2s y)
{
	__builtin_pulp_exor2(x, y);
}
v4s __EXOR4(v4s x, v4s y)
{
	__builtin_pulp_exor4(x, y);
}
v2u __SRL2(v2s x, v2s y)
{
	__builtin_pulp_srl2(x, y);
}
v4u __SRL4(v4s x, v4s y)
{
	__builtin_pulp_srl4(x, y);
}
v2s __SRA2(v2s x, v2s y)
{
	__builtin_pulp_sra2(x, y);
}
v4s __SRA4(v4s x, v4s y)
{
	__builtin_pulp_sra4(x, y);
}
v2s __SLL2(v2s x, v2s y)
{
	__builtin_pulp_sll2(x, y);
}
v4s __SLL4(v4s x, v4s y)
{
	__builtin_pulp_sll4(x, y);
}
int __MAC(int Acc, int x, int y)
{
	__builtin_pulp_mac((x), (y), (Acc));
}
int __MSU(int Acc, int x, int y)
{
	__builtin_pulp_msu((x), (y), (Acc));
}
int __MACS(int Acc, int x, int y)
{
	__builtin_pulp_macs((x), (y), (Acc));
}
int __MACHHS(int Acc, int x, int y)
{
	__builtin_pulp_machhs((x), (y), (Acc));
}
unsigned int __MACU(unsigned int Acc, int x, int y)
{
	__builtin_pulp_macu((x), (y), (Acc));
}
unsigned int __MACHHU(unsigned int Acc, int x, int y)
{
	__builtin_pulp_machhu((x), (y), (Acc));
}
int __MACSN(int Acc, int x, int y, int n)
{
	__builtin_pulp_macsN((x), (y), (Acc), (n));
}
unsigned int __MACUN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_macuN((x), (y), (Acc), (n));
}
int __MACSRN(int Acc, int x, int y, int n)
{
	__builtin_pulp_macsRN((x), (y), (Acc), (n), (1<<((n)-1)));
}
unsigned int __MACURN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_macuRN((x), (y), (Acc), (n), (1<<((n)-1)));
}
int __MACHHSN(int Acc, int x, int y, int n)
{
	__builtin_pulp_machhsN((x), (y), (Acc), (n));
}
unsigned int __MACHHUN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_machhuN((x), (y), (Acc), (n));
}
int __MACHHSRN(int Acc, int x, int y, int n)
{
	__builtin_pulp_machhsN((x), (y), (Acc), (n), (1<<((n)-1)));
}
unsigned int __MACHHURN(unsigned int Acc, int x, int y, int n)
{
	__builtin_pulp_machhuN((x), (y), (Acc), (n), (1<<((n)-1)));
}
int __MULS(int x, int y)
{
	__builtin_pulp_muls((x), (y));
}
unsigned int __MULU(int x, int y)
{
	__builtin_pulp_mulu((x), (y));
}
int __MULHHS(int x, int y)
{
	__builtin_pulp_mulhhs((x), (y));
}
unsigned int __MULHHU(int x, int y)
{
	__builtin_pulp_mulhhu((x), (y));
}
int __MULSN(int x, int y, int n)
{
	__builtin_pulp_mulsN((x), (y), (n));
}
int __MULSRN(int x, int y, int n)
{
	__builtin_pulp_mulsRN((x), (y), (n), (1<<((n)-1)));
}
unsigned int __MULUN(int x, int y, int n)
{
	__builtin_pulp_muluN((x), (y), (n));
}
unsigned int __MULURN(int x, int y, int n)
{
	__builtin_pulp_muluRN((x), (y), (n), (1<<((n)-1)));
}
int __MULHHSN(int x, int y, int n)
{
	__builtin_pulp_mulhhsN((x), (y), (n));
}
int __MULHHSRN(int x, int y, int n)
{
	__builtin_pulp_mulhhsRN((x), (y), (n), (1<<((n)-1)));
}
unsigned int __MULHHUN(int x, int y, int n)
{
	__builtin_pulp_mulhhuN((x), (y), (n));
}
unsigned int __MULHHURN(int x, int y, int n)
{
	__builtin_pulp_mulhhuRN((x), (y), (n), (1<<((n)-1)));
}
int __DOTP2(v2s x, v2s y)
{
	__builtin_pulp_dotsp2((x), (y));
}
unsigned int __DOTPU2(v2s x, v2s y)
{
	__builtin_pulp_dotup2((x), (y));
}
int __DOTPUS2(v2s x, v2s y)
{
	__builtin_pulp_dotusp2((x), (y));
}
int __DOTPSC2(v2s x, int y)
{
	__builtin_pulp_dotspsc2((x), (y));
}
unsigned int __DOTPUSC2(v2s x, int y)
{
	__builtin_pulp_dotupsc2((x), (y));
}
int __DOTPUSSC2(v2s x, int y)
{
	__builtin_pulp_dotuspsc2((x), (y));
}
int __SUMDOTP2(v2s x, v2s y, int z)
{
	__builtin_pulp_sdotsp2((x), (y), (z));
}
unsigned int __SUMDOTPU2(v2s x, v2s y, unsigned int z)
{
	__builtin_pulp_sdotup2((x), (y), (z));
}
int __SUMDOTPUS2(v2s x, v2s y, int z)
{
	__builtin_pulp_sdotusp2((x), (y), (z));
}
int __SUMDOTPSC2(v2s x, int y, int z)
{
	__builtin_pulp_sdotspsc2((x), (y), (z));
}
unsigned int __SUMDOTPUSC2(v2s x, int y, unsigned int z)
{
	__builtin_pulp_sdotupsc2((x), (y), (z));
}
int __SUMDOTPUSSC2(v2s x, int y, int z)
{
	__builtin_pulp_sdotuspsc2((x), (y), (z));
}
int __DOTP4(v4s x, v4s y)
{
	__builtin_pulp_dotsp4((x), (y));
}
unsigned int __DOTPU4(v4s x, v4s y)
{
	__builtin_pulp_dotup4((x), (y));
}
int __DOTPUS4(v4s x, v4s y)
{
	__builtin_pulp_dotusp4((x), (y));
}
int __DOTPSC4(v4s x, int y)
{
	__builtin_pulp_dotspsc4((x), (y));
}
unsigned int __DOTPUSC4(v4s x, int y)
{
	__builtin_pulp_dotupsc4((x), (y));
}
int __DOTPUSSC4(v4s x, int y)
{
	__builtin_pulp_dotuspsc4((x), (y));
}
int __SUMDOTP4(v4s x, v4s y, int z)
{
	__builtin_pulp_sdotsp4((x), (y), (z));
}
unsigned int __SUMDOTPU4(v4s x, v4s y, unsigned int z)
{
	__builtin_pulp_sdotup4((x), (y), (z));
}
int __SUMDOTPUS4(v4s x, v4s y, int z)
{
	__builtin_pulp_sdotusp4((x), (y), (z));
}
int __SUMDOTPSC4(v4s x, int y, int z)
{
	__builtin_pulp_sdotspsc4((x), (y), (z));
}
unsigned int __SUMDOTPUSC4(v4s x, int y, unsigned int z)
{
	__builtin_pulp_sdotupsc4((x), (y), (z));
}
int __SUMDOTPUSSC4(v4s x, int y, int z)
{
	__builtin_pulp_sdotuspsc4((x), (y), (z));
}
v2s __CPLXMULS(v2s x, v2s y)
{
	__builtin_pulp_cplxmuls((x), (y));
}
v2s __CPLXMULSDIV2(v2s x, v2s y)
{
	__builtin_pulp_cplxmulsdiv2((x), (y));
}
v2s __CPLXMULSDIV4(v2s x, v2s y)
{
	__builtin_pulp_cplxmulsdiv4((x), (y));
}
v2s __CPLXCONJ(v2s x)
{
	__builtin_pulp_cplx_conj((x));
}
v2s __SUB2ROTMJ(v2s x, v2s y)
{
	__builtin_pulp_sub2rotmj((x), (y));
}
v2s __ADD2DIV2(v2s x, v2s y)
{
	__builtin_pulp_add2div2((x), (y));
}
v2s __ADD2DIV4(v2s x, v2s y)
{
	__builtin_pulp_add2div4((x), (y));
}
v2s __ADD4DIV2(v2s x, v2s y)
{
	__builtin_pulp_add4div2((x), (y));
}
v2s __ADD4DIV4(v2s x, v2s y)
{
	__builtin_pulp_add4div4((x), (y));
}
v2s __SUB2DIV2(v2s x, v2s y)
{
	__builtin_pulp_sub2div2((x), (y));
}
v2s __SUB2DIV4(v2s x, v2s y)
{
	__builtin_pulp_sub2div4((x), (y));
}
v2s __SUB4DIV2(v2s x, v2s y)
{
	__builtin_pulp_sub4div2((x), (y));
}
v2s __SUB4DIV4(v2s x, v2s y)
{
	__builtin_pulp_sub4div4((x), (y));
}
v2s __VITMAX(v2s x, v2s y)
{
	__builtin_pulp_vitmax2((x), (y));
}
v2s __VITSEL(v2s x, v2s y)
{
	__builtin_pulp_vitsel2((x), (y));
}
int __FL1(int x)
{
	__builtin_pulp_fl1((x));
}
int __CLB(int x)
{
	__builtin_pulp_clb((x));
}
int __BITSET(int x, int size, int off)
{
	__builtin_pulp_bset((x), (((1<<(size))-1)<<(off)));
}
int __BITSET_R(int x, int size, int off)
{
	__builtin_pulp_bset_r((x), ExtInsMaskFast((size), (off)));
}
int __BITSET_R_SAFE(int x, int size, int off)
{
	__builtin_pulp_bset_r((x), ExtInsMaskSafe((size), (off)));
}
int __BITCLR(int x, int size, int off)
{
	__builtin_pulp_bclr((x), ~(((1<<(size))-1)<<(off)));
}
int __BITCLR_R(int x, int size, int off)
{
	__builtin_pulp_bclr_r((x), ExtInsMaskFast((size), (off)));
}
int __BITCLR_R_SAFE(int x, int size, int off)
{
	__builtin_pulp_bclr_r((x), ExtInsMaskSafe((size), (off)));
}
int __BITEXTRACT(int x, int size, int off)
{
	__builtin_pulp_bextract((x), (size), (off));
}
unsigned int __BITEXTRACTU(int x, int size, int off)
{
	__builtin_pulp_bextractu((x), (size), (off));
}
int __BITEXTRACT_R(int x, int size, int off)
{
	__builtin_pulp_bextract_r((x), ExtInsMaskFast((size), (off)));
}
unsigned int __BITEXTRACTU_R(int x, int size, int off)
{
	__builtin_pulp_bextractu_r((x), ExtInsMaskFast((size), (off)));
}
int __BITEXTRACT_R_SAFE(int x, int size, int off)
{
	__builtin_pulp_bextract_r((x), ExtInsMaskSafe((size), (off)));
}
unsigned int __BITEXTRACTU_R_SAFE(int x, int size, int off)
{
	__builtin_pulp_bextractu_r((x), ExtInsMaskSafe((size), (off)));
}
int __BITINSERT(int dst, int src, int size, int off)
{
	__builtin_pulp_binsert((dst), ~(((1<<(size))-1)<<(off)), (src), (((1<<(size))-1)<<(off)), (off));
}
int __BITINSERT_R(int dst, int src, int size, int off)
{
	__builtin_pulp_binsert_r((dst), (src), ExtInsMaskFast((size), (off)));
}
int __BITINSERT_R_SAFE(int dst, int src, int size, int off)
{
	__builtin_pulp_binsert_r((dst), (src), ExtInsMaskSafe((size), (off)));
}
int __ROTR(int x)
{
	__builtin_pulp_rotr((x));
}
unsigned int __ADDNORMU(int x, int y, int scale)
{
	__builtin_pulp_adduN((x), (y), (scale));
}
unsigned int __ADDNORMU_REG(int x, int y, int scale)
{
	__builtin_pulp_adduN_r((x), (y), (scale));
}
int __ADDNORM(int x, int y, int scale)
{
	__builtin_pulp_addN((x), (y), (scale));
}
int __ADDNORM_REG(int x, int y, int scale)
{
	__builtin_pulp_addN_r((x), (y), (scale));
}
unsigned int __ADDROUNDNORMU(int x, int y, int scale)
{
	__builtin_pulp_adduRN((x), (y), (scale), (1<<((scale)-1)));
}
unsigned int __ADDROUNDNORMU_REG(int x, int y, int scale)
{
	__builtin_pulp_adduRN_r((x), (y), (scale));
}
int __ADDROUNDNORM(int x, int y, int scale)
{
	__builtin_pulp_addRN((x), (y), (scale), (1<<((scale)-1)));
}
int __ADDROUNDNORM_REG(int x, int y, int scale)
{
	__builtin_pulp_addRN_r((x), (y), (scale));
}
unsigned int __SUBNORMU(int x, int y, int scale)
{
	__builtin_pulp_subuN((x), (y), (scale));
}
unsigned int __SUBNORMU_REG(int x, int y, int scale)
{
	__builtin_pulp_subuN_r((x), (y), (scale));
}
int __SUBNORM(int x, int y, int scale)
{
	__builtin_pulp_subN((x), (y), (scale));
}
int __SUBNORM_REG(int x, int y, int scale)
{
	__builtin_pulp_subN_r((x), (y), (scale));
}
unsigned int __SUBROUNDNORMU(int x, int y, int scale)
{
	__builtin_pulp_subuRN((x), (y), (scale), (1<<((scale)-1)));
}
unsigned int __SUBROUNDNORMU_REG(int x, int y, int scale)
{
	__builtin_pulp_subuRN_r((x), (y), (scale));
}
int __SUBROUNDNORM(int x, int y, int scale)
{
	__builtin_pulp_subRN((x), (y), (scale), (1<<((scale)-1)));
}
int __SUBROUNDNORM_REG(int x, int y, int scale)
{
	__builtin_pulp_subRN_r((x), (y), (scale));
}
unsigned int __ROUNDNORMU(int x, int scale)
{
	__builtin_pulp_adduRN((x), 0, (scale), (1<<((scale)-1)));
}
int __ROUNDNORMU_REG(int x, int scale)
{
	__builtin_pulp_adduRN_r((x), 0, (scale));
}
unsigned int __ROUNDNORM(int x, int scale)
{
	__builtin_pulp_addRN((x), 0, (scale), (1<<((scale)-1)));
}
int __ROUNDNORM_REG(int x, int scale)
{
	__builtin_pulp_addRN_r((x), 0, (scale));
}
int __COREID()
{
	__builtin_pulp_CoreId();
}
int __CLUSTERID()
{
	__builtin_pulp_ClusterId();
}
int __NCORE()
{
	__builtin_pulp_CoreCount();
}
int __ISFC()
{
	__builtin_pulp_IsFc();
}
void __SPRWRITE(int x, int y)
{
	__builtin_pulp_spr_write(x, y);
}
int __SPRREAD(int x)
{
	__builtin_pulp_spr_read(x);
}
int __SPRREAD_VOL(int x)
{
	__builtin_pulp_spr_read_vol(x);
}
void __SPRBITSET(int spr, int off)
{
	__builtin_pulp_spr_bit_set((spr), (off));
}
void __SPRBITCLR(int spr, int off)
{
	__builtin_pulp_spr_bit_clr((spr), (off));
}
int __SPRREADTHENWRITE(int spr, int x)
{
	__builtin_pulp_read_then_spr_write((spr), (x));
}
int __SPRREADTHENBITSET(int spr, int off)
{
	__builtin_pulp_read_then_spr_bit_set((spr), (off));
}
int __SPRREADTHENBITCLR(int spr, int off)
{
	__builtin_pulp_read_then_spr_bit_clr((spr), (off));
}
int __READ_BASE_OFF(int * base, int off)
{
	__builtin_pulp_read_base_off((base), (off));
}
void __WRITE_BASE_OFF(int * base, int off, int val)
{
	__builtin_pulp_write_base_off((base), (off), (val));
}
int __READ_BASE_OFF_VOL(int * base, int off)
{
	__builtin_pulp_OffsetedRead((base), (off));
}
short int __READ_BASE_OFF_HALF_VOL(short int * base, int off)
{
	__builtin_pulp_OffsetedReadHalf((base), (off));
}
signed char __READ_BASE_OFF_BYTE_VOL(signed char * base, int off)
{
	__builtin_pulp_OffsetedReadByte((base), (off));
}
void __WRITE_BASE_OFF_VOL(int x, int * base, int off)
{
	__builtin_pulp_OffsetedWrite((x), (base), (off));
}
void __WRITE_BASE_OFF_HALF_VOL(int x, short int * base, int off)
{
	__builtin_pulp_OffsetedWriteHalf((x), (base), (off));
}
void __WRITE_BASE_OFF_BYTE_VOL(int x, signed char * base, int off)
{
	__builtin_pulp_OffsetedWriteByte((x), (base), (off));
}
