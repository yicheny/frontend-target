import { useMemo, useState, useEffect } from 'react'
import { Tab, TabItem, Messager } from 'rootnet-ui'
import { useRivalOptions, useTargetOptions, useTradingBookOptionsEditPermission, useContractSwapList } from '../../../common/options'
import { getGlobalConstSelectOpsObject } from '../../../services'
import Position from './position'
import Contract from './contract'
import { useLocation } from 'react-router-dom'
import { usePathSearch } from '../../../common/hooks'

function Page() {
    const ReportComponent = useModeInfo()
    const selectOption = useFetchSelectOption()

    return <ReportComponent selectOption={selectOption} />
}

export default Page

function useModeInfo() {
    const { pathname } = useLocation()
    const routerParma = usePathSearch() // 券池余量统计页，调转到当前页携带的参数

    return useMemo(() => {
        const PageMode = {
            daytime: (props) => <ValuationResult {...props} pageMode='daytime' />,
            reckoning: (props) => <ValuationResult {...props} pageMode='reckoning' routerParma={routerParma} />,
        }

        if (pathname === '/swap/valuation-result') return PageMode['daytime']
        return PageMode['reckoning']
    }, [pathname, routerParma])
}

function ValuationResult({ pageMode, routerParma }) {
    const selectOption = useFetchSelectOption(pageMode)
    const [active, setActive] = useState()
    return (
        <div className='tab-wrapper x flex-y fill'>
            <Tab active={active} onChange={setActive}>
                <TabItem header='持仓价值'>
                    <Position selectOption={selectOption} pageMode={pageMode} routerParma={routerParma} />
                </TabItem>
                <TabItem header='合约价值'>
                    <Contract selectOption={selectOption} pageMode={pageMode} />
                </TabItem>
            </Tab>
        </div>
    )
}

function useFetchSelectOption() {
    const counterParty = useRivalOptions()
    const underlying = useTargetOptions()
    const tradingBook = useTradingBookOptionsEditPermission()
    const contractIdList = useContractSwapList()
    const [swapPaymentType, setSwapPaymentType] = useState([])

    useEffect(() => {
        const fetch = async () => {
            try {
                const info = await getGlobalConstSelectOpsObject(['swappaymentType'])
                setSwapPaymentType(info || [])
            } catch (err) {
                const msg = err.message || err._message
                Messager.show(msg, { icon: 'error' })
            }
        }
        fetch()
    }, [])
    return { contractIdList, ...global, counterParty, underlying, tradingBook, ...swapPaymentType }
}
