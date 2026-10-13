"""Optional maintainer tool, not required to build/run the C++ lessons.
使用独立py3gpp/Sionna稀疏矩阵编码路径生成向量。
Use the independent py3gpp/Sionna sparse-matrix encoding path.
Requires py3gpp==0.6.0. See docs/validation.md for the small-BG2 caveat.
"""
from pathlib import Path
import hashlib
import json
import numpy as np
import py3gpp
from py3gpp.nrDLSCHInfo import getZarray
from py3gpp.nrLDPCEncode import _load_basegraph, _gen_submat, _encode

out = Path(__file__).resolve().parents[1] / 'tests/data'
out.mkdir(exist_ok=True)
def write(name, data):
    np.savetxt(out / name, np.asarray(data).reshape(-1), fmt='%d')
write('gold.txt', py3gpp.nrPRBS(12345, 256))
bits = np.array([(i * 17 + i // 3) % 2 for i in range(101)])
write('crc_input.txt', bits)
for kind in ['16','24A','24B']:
    write('crc_'+kind+'.txt', py3gpp.nrCRCEncode(bits,kind))
for qm,mod in [(2,'QPSK'),(4,'16QAM')]:
    labels = np.array([(word >> (qm-1-i)) & 1 for word in range(2**qm) for i in range(qm)])
    symbols = py3gpp.nrSymbolModulate(labels,mod)
    np.savetxt(out/f'qam{qm}.txt', np.c_[symbols.real,symbols.imag], fmt='%.12g')
# Exercise all eight lifting sets and both base graphs, including small BG2.
manifest=[]
for bg in [1,2]:
    for z in [8,12,20,28,36,44,52,60,384]:
        k=(22 if bg==1 else 10)*z
        kp=k-7
        msg=np.array([(i*13+i//7)%2 for i in range(k)],dtype=int)
        msg[kp:]=0
        ils=next(i for i,group in enumerate(getZarray()) if z in group)
        bm=_load_basegraph(ils,bg)
        # nrLDPCEncode's small-BG2 iLS selection uses K instead of Zc.
        # Select iLS explicitly; use its independent sparse matrix algorithm.
        a,b,c,d=_gen_submat(bm,22 if bg==1 else 10,z,bg)
        full=_encode(msg,a,b,c,d)
        tag=f'bg{bg}_z{z}'
        write(tag+'_input.txt',msg)
        write(tag+'_full.txt',full)
        punctured=full[2*z:].astype(int)
        punctured[kp-2*z:k-2*z]=-1
        e=4*((k+111+3)//4)
        for rv in range(4):
            write(f'{tag}_rv{rv}.txt',py3gpp.nrRateMatchLDPC(punctured[:,None],e,rv,'16QAM',1))
        manifest.append([bg,z,kp,e])
np.savetxt(out/'ldpc_cases.txt',manifest,fmt='%d')
# Independently segment TBs, including the CRC16/CRC24A threshold and C>1.
for a, bg in [(24,2),(576,2),(3824,2),(4000,1),(9000,1)]:
    tb=np.array([(i*11+i//5)%2 for i in range(a)],dtype=int)
    b=py3gpp.nrCRCEncode(tb,'24A' if a>3824 else '16').reshape(-1)
    cbs=py3gpp.nrCodeBlockSegmentLDPC(b,bg)
    write(f'tb{a}_input.txt',tb)
    np.savetxt(out/f'tb{a}_blocks.txt',cbs.T,fmt='%d')
# TBS oracle for the supported profile; independent package, same standard table.
rates=[120,157,193,251,308,379,449,526,602,679,340,378,434,490,553,616,658]
tbs=[]
for prbs in [1,6,20]:
    for mcs,r in enumerate(rates):
        tbs.append([prbs,mcs,py3gpp.nrTBS('QPSK' if mcs<10 else '16QAM',1,prbs,162,r/1024)])
np.savetxt(out/'tbs_cases.txt',tbs,fmt='%d')
metadata={'generator':'py3gpp 0.6.0; sparse-matrix path; explicit iLS',
          'files':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(out.glob('*.txt'))}}
(out/'manifest.json').write_text(json.dumps(metadata,indent=2)+'\n')
