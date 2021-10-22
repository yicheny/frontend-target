import { hasData } from 'rootnet-core/system'
import { useMemo } from 'react'
import { addNoCol, getNow, longToYMD } from '../../../../common/utils'
import { DataChange, N0, N2, N4, Secformat } from 'rootnet-core/format'
import { curryList } from '../widgets/components'
import { curryUseGetData } from '../widgets/hooks'

const bsTypes = { B: '多头', S: '空头' }
const rateTypes = { 0: '固定' }

function translate(dict) {
  if (!hasData(dict)) throw new Error('dict必须是有值的数组或对象')
  return (key) => dict[key]
}

function useColumns() {
  return useMemo(() => {
    return addNoCol([
      { header: '估值日', bind: 'evaluationDate', width: 100, convert: longToYMD, csvConvert: true },
      { header: '对手方', bind: 'counterpartyName', width: 120, tooltip: true },
      { header: '对手方交易账户', bind: 'tradingAcctName', width: 140, tooltip: true },
      { header: '合约编号', bind: 'contractId', width: 140, tooltip: true },
      { header: '起始日', bind: 'startDate', width: 100, convert: longToYMD, csvConvert: true },
      { header: '到期日', bind: 'matureDate', width: 100, convert: longToYMD, csvConvert: true },
      { header: '标的代码', bind: 'displayName', width: 140, tooltip: true },
      { header: '对手方多空方向', bind: 'bsType', tooltip: true, csvConvert: true, convert: translate(bsTypes) },
      { header: '期初价格', bind: 'netBeginPrice', align: 'right', width: 100, tooltip: true, csvConvert: true, convert: N4 },
      {
        header: '期初名义数量',
        bind: 'initialNotionalQty',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N0,
        csvConvert: true,
      },
      { header: '名义数量', bind: 'notionalQty', align: 'right', width: 100, tooltip: true, convert: N0, csvConvert: true },
      {
        header: '期初名义本金',
        bind: 'initialNotionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      { header: '名义本金', bind: 'notionalPrincipal', align: 'right', width: 100, tooltip: true, convert: N2, csvConvert: true },
      { header: '盯市价格', bind: 'mtMPrice', align: 'right', width: 100, tooltip: true, convert: N4, csvConvert: true },
      {
        header: '标的市值',
        bind: 'underlyingMarketValue',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '权益端待实现收益·浮动',
        bind: 'equityRevenueFloat',
        align: 'right',
        width: 160,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '权益端待实现收益·分红',
        bind: 'equityRevenueDividends',
        align: 'right',
        width: 160,
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
      { header: '利率类型', bind: 'interestRateType', width: 80, tooltip: true, csvConvert: true, convert: translate(rateTypes) },
      {
        header: '利率',
        bind: 'interestRate',
        align: 'right',
        width: 80,
        tooltip: true,
        convert: (v) => {
          let _v = v ? Number(v) * 100 : 0
          return N4(_v)
        },
        csvConvert: true,
      },
      {
        header: '利率端待实现收益·券息',
        bind: 'interestRateRevenueCoupon',
        align: 'right',
        width: 160,
        tooltip: true,
        convert: N2,
        csvConvert: true,
      },
      {
        header: '利率端待实现收益·费用',
        bind: 'interestRateRevenueFees',
        align: 'right',
        width: 160,
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
      {
        header: '合约持仓价值',
        bind: 'totalUnRealizedPNL',
        align: 'right',
        width: 120,
        convert: N2,
        csvConvert: true,
        tooltip: true,
      },
      {
        header: '累计已实现盈亏',
        bind: 'totalRealizedPNL',
        align: 'right',
        width: 120,
        convert: N2,
        csvConvert: true,
        tooltip: true,
      },
      { header: '估值更新时间', bind: 'lastEvTime', width: 150, convert: Secformat, csvConvert: true },
    ])
  }, [])
}

export function getDefaultParams() {
  return {
    evDate: DataChange(getNow()),
    underlyingId: null,
    contractIdList: [],
    counterpartyIdList: [],
  }
}

export function createListWithUrl(url) {
  return curryList({
    title: '持仓价值',
    useColumns: useColumns,
    useGetData: curryUseGetData(url),
  })
}
