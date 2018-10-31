//=============================================================================
// Local Base에 장착된 Module의 정보를 수집
// <주의> 지연 요소가 많은 Subroutine (12 Slot의 경우 약 300uSec 지연 발생되니, 너무 자주 호출하지 말 것)
void CollectCardID(void)
{
	u16 i, j, id, addr;
	
    if(f_skip_CollectCardID == 1)   
    {
        f_skip_CollectCardID = 0;
        return;
    }
    // 로더통신에서 호출할 경우 바로 실행하지 않고 ISR 완료 후 EtherMan에서 실행한다., 이현규, 2017-04-19
	if(fLoaderComm == 1)
    {
        fReqCollectCardID = 1;
        return;
    }
    
	// Base 인식이 정상적이지 못할 경우 그냥 return
	if(BaseInfo[0].BaseType > MAX_SLOT_COUNT)
		return;
	// 모든 Slot Scan 실시
	addr = 0;
    
    // 모듈정보 초기화 
	for(i=0; i<MAX_EXPANSION;i++)
    {
	    for(j=0; j<MAX_SLOT_COUNT;j++)
        {
            ExpModule[i][j].ModuleType = 0xff;
        }
    }
    
	for(i=0; i<BaseInfo[0].BaseType; i++)
	{
		// 슬롯 ID를 정상적으로 읽어내기 위해서는 최소 25uSec. 지연이 필요함. (오실로스코프로 확인한 사실)
		// 특히, 이번 슬롯이 빈 슬롯인 경우, Data Bus 라인들이 Pull-Up 완료 될 때 까지 이 정도의 시간이 소요됨.
		// (빈 슬롯은 Card ID가 0xFF로 읽혀진다.)
        
		for(j=0; j<600; j++);
        
		// 모듈 ID 읽기
		id = GetCardID(i);
		// 모듈 정보 초기화
		ExpModule[0][i].ModuleType = id;
        // 모듈 ID 정보 데이터 추가, F13~24, 이현규, 2017-01-29 
        IramDevMem.F[FMEM_OFFSET_MODULE_ID + i].Word = id;
        
		ExpModule[0][i].IoCapa = CardDB[id].IoCapacity;
		ExpModule[0][i].fMounted = (id!=CARD_TYPE_NONE) ? 1 : 0;
		ExpModule[0][i].fTo = 0;
		ExpModule[0][i].DetectCnt = 0;
		for(j=0; j<16; j++)
			ExpModule[0][i].ToBmp[j] = 0;
		ExpModule[0][i].CardAddr = addr;						// 임시 카드 어드레스 할당 : RUN 모드 진입시 CPU Paramter를 사용하여 재할당 할 것임.
		ExpModule[0][i].DpState = DPSTATE_NOT_MOUNTED;
		ExpModule[0][i].TimeoutCnt = 0;
		// IO 예약이 없을 경우 적용되는 카드 어드레스 할당 (임시)
		addr += CardDB[id].IoCapacity / 16;
            
        // 신형 I/O 모둘일 경우 버전정보를 읽어온다., 장종미, 2017-06-01
        if((CardDB[ExpModule[0][i].ModuleType].Type == CARD_TYPE_INTEL_INPUT) ||    // 신형 입력 
           (CardDB[ExpModule[0][i].ModuleType].Type == CARD_TYPE_INTEL_OUTPUT) ||   // 신형 출력  
           (CardDB[ExpModule[0][i].ModuleType].Type == CARD_TYPE_INTEL_INOUT) ||    // 신형 입출력, 구형 DIO 모듈 code에서 신형 code로 변경, 조은애, 2018-10-01
           (ExpModule[0][i].ModuleType == 0xF2))    // RM01B
        {
            ReadIntelVersion(i);
        }
            
	}
    
    /* 이중화 지원 모델일 경우 */
#ifndef OPTION_NO_REDUNDANT
	InitRedundant();
    // 이중화 통신모듈 DPRAM 초기화 구문 InitRedundant() 함수 안으로 이동, 이현규, 2018-05-07 
#endif
    
	// Parameter에 설정된 I/O 예약 상태를 적용한다.
	if(!CpuParam[0].nRunit)
    {
        /* I/O 예약 처리 */
		// 파라미터에서 증설 설정 안되어있으면 Local Base만 Allocation 실시		
		IoAllocation();
    }
	else
    {
#ifndef OPTION_NO_REDUNDANT
        // 이중화 모듈일 경우 백업모드라면 증설 초기화를 하지 않고 IO예약만 처리한다.
        if(FlagBackup)		// FlagSecondary->FlagBackup으로 변경, 김다현 2018-02-09
        {
            // 이중화 동작 시 Active/Bacpup 간 모듈정보 불일치 에러 발생으로 IoAllocation() 주석해제함, 이현규, 2017-10-10
            IoAllocation();
        }
        else
        {
            //IoAllocation(); 
            // 증설 라인이 살아있지 않을 때에만 증설 초기화를 한다. 이미 Secondary가 Active로 동작하고 있는 경우 Primary에서 증설초기화 시도시 문제가 발생할 수 있다. 김다현 2018-03-05
            if(!ExpLineDetect)
                InitExpansion(5);
        }
        RedInitTmr = 0;
		return;				// Primary 전원 Off 상태에서 Secondary 전원 On 시켰을 경우 f_skip_CollectCardID를 Set 하면 안된다. 여기서 리턴하도록 한다. 김다현 2018-03-02
#else
        /* 증설 초기화 */
		// 증설 초기화 실시한 다음 증설베이스와 모듈 정보를 바탕으로 I/O 예약 상태를 적용한다.
		// (아래 함수 내부에서 IoAllocation() 함수가 호출 됨)
		InitExpansion(5);
#endif
    }
    f_skip_CollectCardID = 1;   // 증설 초기화를 했으면 다시 증설 초기화를 하기위한 조건이 될 때 까지 중복해서 증설초기화를 하지 않는다., 이현규, 207-08-29 
}
