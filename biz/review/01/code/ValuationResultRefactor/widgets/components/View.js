import { ViewProvider } from '../../../../../common/context'
import { Tab, TabItem } from 'rootnet-ui'
import { curryGlobalConstOptions } from '../../../../../services/hooks'
import { useGet } from 'rootnet-biz/es/hooks'
import { useEffect, useMemo } from 'react'
import _ from 'lodash'
import { useContractSwapList, useRivalOptions, useTargetOptions, useTradingBookOptionsEditPermission } from '../../../../../common/options'

export function createView({ TabContent1, TabContent2 }) {
  return function View() {
    return (
      <ViewProvider value={useOptions()}>
        <div className='tab-wrapper x flex-y fill'>
          <Tab active={0}>
            <TabItem header='持仓价值'>
              <TabContent1 />
            </TabItem>
            <TabItem header='合约价值'>
              <TabContent2 />
            </TabItem>
          </Tab>
        </div>
      </ViewProvider>
    )
  }
}

const useSwappaymentType = curryGlobalConstOptions('swappaymentType')

function useContractLimitOptions() {
  const { data, doFetch } = useGet()

  useEffect(() => {
    doFetch('contract/swap/queryContractList?contractStatusList=0,1,2,9')
  }, [doFetch])

  return useMemo(() => {
    return _.map(data, (item) => ({
      text: `${item.contractId}-${item.contractName}`,
      value: item.contractId,
      source: item,
    }))
  }, [data])
}

function useOptions() {
  return {
    counterPartyOptions: useRivalOptions(),
    underlyingOptions: useTargetOptions(),
    tradingBookOptions: useTradingBookOptionsEditPermission(),
    contractIdOptions: useContractSwapList(),
    swapPaymentTypeOptions: useSwappaymentType(),
    underlyingIdOptions: useTargetOptions(),
    contractLimitOptions: useContractLimitOptions(),
  }
}
