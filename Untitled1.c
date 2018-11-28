u16 SendDynamicData(void)
{
    u16 Size, i;
    u16 *pDst, *pSrc;
    u8 BlockNum;

    // 예외처리 : 동적데이터 동기화를 수행하지 않는 경우
    if(FlagStopMode || (RedundantFlags&RC_FLAG_SWITCH_OVER))
        return 0;
    
    GetSyncBlockNumber();
    
    BlockNum = CurrDynamicDataBlockNum;
    i = BlockNum = 0;
    TxBuf[19] = REDUN_COMM_DYNAMIC_DATA;
    TxBuf[20] = BlockNum;
    TxBuf[21] = 0;
    TxBuf[22] = 0;
    pDst = (u16 *)&TxBuf[23];
    
    if(BlockNum <= ETH_DYNA_SYNC_D_LAST_BLOCK)
    {
        Size = (BlockNum==ETH_DYNA_SYNC_D_LAST_BLOCK) ? 610 : 730;
        pSrc = (u16 *)&DevMem.D[i*730];
        
        for(i=0; i<Size; i++)
            *pDst++ = *pSrc++;
        
        Size *= 2;
    }
    else
    {
        switch(BlockNum)
        {
            case ETH_DYNA_SYNC_M:
            case ETH_DYNA_SYNC_M+1:      
                Size = 500;
                i = BlockNum - ETH_DYNA_SYNC_M;
                pSrc = (u16 *)&DevMem.M[i*500];
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_L:
            case ETH_DYNA_SYNC_L+1:      
                Size = 500;
                i = BlockNum - ETH_DYNA_SYNC_L;
                pSrc = (u16 *)&DevMem.L[i*500];
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_K:
            case ETH_DYNA_SYNC_K+1:      
                Size = 500;
                i = BlockNum - ETH_DYNA_SYNC_K;
                pSrc = (u16 *)&DevMem.K[i*500];
                
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_TCS:
                Size = 562;
                pSrc = (u16 *)&DevMem.T;
                
                for(i=0; i<512; i++)
                    *pDst++ = *pSrc++;
                
                pSrc = (u16 *)&DevMem.S;
                
                for(i=0; i<50; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_TC:
            case ETH_DYNA_SYNC_TC+1:      
            case ETH_DYNA_SYNC_TC+2:      
            case ETH_DYNA_SYNC_TC+3:      
            case ETH_DYNA_SYNC_TC+4:      
            case ETH_DYNA_SYNC_TC+5:      
                Size = (i==56) ? 446 : 730;
                i = BlockNum - ETH_DYNA_SYNC_TC;
                pSrc = (u16 *)&DevMem.TC[i*730];
                
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_CC:
            case ETH_DYNA_SYNC_CC+1:      
            case ETH_DYNA_SYNC_CC+2:      
            case ETH_DYNA_SYNC_CC+3:      
            case ETH_DYNA_SYNC_CC+4:      
            case ETH_DYNA_SYNC_CC+5:            
                Size = (i==62) ? 446 : 730;
                i = BlockNum - ETH_DYNA_SYNC_CC;
                pSrc = (u16 *)&DevMem.CC[i*730];
                
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_Z:
            case ETH_DYNA_SYNC_Z + 1:
                Size = 512;
                i = BlockNum - ETH_DYNA_SYNC_Z;
                pSrc = (u16 *)&DevMem.Z[i*512];
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_X:
                Size = MEMSIZE_X;
                pSrc = (u16 *)&IramDevMem.X;
                
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_Y:
                Size = MEMSIZE_Y;
                pSrc = (u16 *)&IramDevMem.Y;
                
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_TS:
            case ETH_DYNA_SYNC_TS+1:      
            case ETH_DYNA_SYNC_TS+2:      
            case ETH_DYNA_SYNC_TS+3:      
            case ETH_DYNA_SYNC_TS+4:      
            case ETH_DYNA_SYNC_TS+5:            
                Size = (i==72) ? 446 : 730;
                i = BlockNum - ETH_DYNA_SYNC_TS;
                pSrc = (u16 *)&DevMem.TS[i*730];
                
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_CS:
            case ETH_DYNA_SYNC_CS+1:      
            case ETH_DYNA_SYNC_CS+2:      
            case ETH_DYNA_SYNC_CS+3:      
            case ETH_DYNA_SYNC_CS+4:            
            case ETH_DYNA_SYNC_CS+5:                  
                Size = (i==78) ? 446 : 730;
                i = BlockNum - ETH_DYNA_SYNC_CS;
                pSrc = (u16 *)&DevMem.CS[i*730];
                
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_Q:
                Size = 512;
                pSrc = (u16 *)&IramDevMem.Q[0];
                for(i=0; i<Size; i++)
                    *pDst++ = *pSrc++;
                Size *= 2;
                break;
                
            case ETH_DYNA_SYNC_VAR:
                RedDynBuf.TimeStamp = MyRedTstmp;
                RedDynBuf.TmrCnt10 = TmrCnt10;
                RedDynBuf.SysClockBase = SysClockBase + 1;					// 전달 Delay를 고려, 1 카운터를 보정해 준다 (보정시간 : 약 10mS)
                RedDynBuf.FmemSysClock = IramDevMem.F[FMEM_OFFSET_SYSCLOCK].Word;

                pSrc = (u16 *)&RedDynBuf;
                for(i=0; i<sizeof(RedDynBuf); i+=2)
                    *pDst++ = *pSrc++;
                
                // RTC 데이터
                *pDst++ = IramDevMem.F[FMEM_OFFSET_RTC_YEAR].Word;
                *pDst++ = IramDevMem.F[FMEM_OFFSET_RTC_MONDATE].Word;
                *pDst++ = IramDevMem.F[FMEM_OFFSET_RTC_DAYHOUR].Word;
                *pDst++ = IramDevMem.F[FMEM_OFFSET_RTC_MINSEC].Word;
                
                Size = sizeof(RedDynBuf) + 8;   //  RTC 데이터 8바이트 추가 
                break;

            default:
                break;        
        }
    }
    Size += 4;
    return Size;
}