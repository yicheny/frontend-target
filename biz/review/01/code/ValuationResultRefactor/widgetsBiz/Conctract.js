import { DataChange, N2, Secformat } from 'rootnet-core/format'
import { addNoCol, getNow, longToYMD } from '../../../../common/utils'
import { curryList } from '../widgets/components'
import { curryUseGetData } from '../widgets/hooks'
import { useMemo } from 'react'

function useColumns() {
  return useMemo(() => {
    return addNoCol([
      { header: '估值日', bind: 'evaluationDate', width: 100, convert: longToYMD, csvConvert: true },
      { header: '交易簿', bind: 'tradingBookName', width: 120, tooltip: true },
      { header: '对手方', bind: 'counterpartyName', width: 120, tooltip: true },
      { header: '对手方交易账户', bind: 'tradingAcctName', width: 140, tooltip: true },
      { header: '合约编号', bind: 'contractId', width: 140, tooltip: true },
      { header: '产品类型', bind: 'productType', width: 80, tooltip: true },
      { header: '起始日', bind: 'startDate', width: 100, convert: longToYMD, csvConvert: true },
      { header: '到期日', bind: 'matureDate', width: 100, convert: longToYMD, csvConvert: true },
      {
        header: '合约名义本金',
        bind: 'notionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '多头名义本金',
        bind: 'longNotionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '空头名义本金',
        bind: 'shortNotionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '多头标的市值',
        bind: 'longMarketValue',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '空头标的市值',
        bind: 'shortMarketValue',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '权益端待实现收益',
        bind: 'equityRevenueSum',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '利率端待实现收益',
        bind: 'interestRateRevenueSum',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      { header: '合约持仓价值', bind: 'underlyingMarketValue', width: 120, convert: N2, csvConvert: true, align: 'right' },
      { header: '当日实现收益', bind: 'tdrealized', align: 'right', width: 130, tooltip: true, convert: N2, csvConvert: true }, // 使用的是利率端实现
      {
        header: '合约已实现收益',
        bind: 'totalRealized',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      { header: '估值更新时间', bind: 'lastEvTime', width: 150, convert: Secformat, csvConvert: true },
      {
        header: '对手方初始保证金',
        bind: 'initMarginAm',
        align: 'right',
        width: 120,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      { header: '对手方维持保证金', bind: 'marginAmt', align: 'right', width: 120, tooltip: true, convert: N2, csvConvert: true },
      { header: '维持保证金计算时间', bind: 'marginCalcTime', width: 160, convert: Secformat, csvConvert: true },
      { header: '投资规模', bind: 'investmentScale', convert: N2, align: 'right' },
    ])
  }, [])
}

export function getDefaultParams() {
  return {
    evDate: DataChange(getNow()),
    tradingBookIdList: [],
    contractIdList: [],
    counterpartyIdList: [],
  }
}

export function createListWithUrl(url) {
  return curryList({
    title: '合约价值',
    useColumns,
    useGetData: curryUseGetData(url),
  })
}
