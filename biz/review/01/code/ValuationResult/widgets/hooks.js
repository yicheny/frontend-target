import { useCallback, useMemo, useEffect } from 'react'
import { usePost } from 'rootnet-biz/lib/hooks'
import { Dateformat, Secformat, N2, N4, N0 } from 'rootnet-core/format'
// import _ from 'lodash'

const formatDate = (e) => Dateformat(e)
// const formatRate = (e) => (isNil(e) ? null : `${_.round(e * 100, 4)}%`)
const bsTypeList = { B: '多头', S: '空头' }
// const regexp = /(?:\.0*|(\.\d+?)0+)$/
// const N4 = (value) => (isNil(value) ? null : _.replace(number_format(value, 'N4'), regexp, '$1'))
const rateType = { 0: '固定' }

export function useGetColumns(mode, swappaymentType) {
  const position = useGetPositionColumn()
  const contract = useGetContractColumn(swappaymentType)
  return useMemo(() => {
    const columns = { position, contract }
    return {
      resizable: true,
      virtualized: false,
      autoFill: true,
      fixedLeft: 1,
      columns: columns[mode](),
      nilText: '-',
      emptyText: '-',
    }
  }, [contract, position, mode])
}

function useGetPositionColumn() {
  return useCallback(() => {
    return [
      { header: '#', convert: (v, o, i) => i + 1, width: 40, align: 'center', stretch: false },
      { header: '估值日', bind: 'evaluationDate', width: 100, convert: formatDate, csvConvert: true },
      { header: '对手方', bind: 'counterpartyName', width: 120, tooltip: true },
      { header: '对手方交易账户', bind: 'tradingAcctName', width: 140, tooltip: true },
      { header: '合约编号', bind: 'contractId', width: 140, tooltip: true },
      { header: '起始日', bind: 'startDate', width: 100, convert: formatDate, csvConvert: true },
      { header: '到期日', bind: 'matureDate', width: 100, convert: formatDate, csvConvert: true },
      { header: '标的代码', bind: 'displayName', width: 140, tooltip: true },
      { header: '本方多空方向', bind: 'bsType', tooltip: true, convert: handleConvert(bsTypeList), csvConvert: true },
      { header: '期初价格', bind: 'netBeginPrice', align: 'right', width: 100, tooltip: true, csvConvert: true, convert: (v) => N4(v) },
      {
        header: '期初名义数量',
        bind: 'initialNotionalQty',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N0(v),
        csvConvert: true,
      },
      { header: '名义数量', bind: 'notionalQty', align: 'right', width: 100, tooltip: true, convert: (v) => N0(v), csvConvert: true },
      {
        header: '期初名义本金',
        bind: 'initialNotionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      { header: '名义本金', bind: 'notionalPrincipal', align: 'right', width: 100, tooltip: true, convert: (v) => N2(v), csvConvert: true },
      { header: '盯市价格', bind: 'mtMPrice', align: 'right', width: 100, tooltip: true, convert: (v) => N4(v), csvConvert: true },
      {
        header: '标的市值',
        bind: 'underlyingMarketValue',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '权益端待实现收益·浮动',
        bind: 'equityRevenueFloat',
        align: 'right',
        width: 160,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '权益端待实现收益·分红',
        bind: 'equityRevenueDividends',
        align: 'right',
        width: 160,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '权益端待实现收益',
        bind: 'equityRevenueSum',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      { header: '利率类型', bind: 'interestRateType', width: 80, tooltip: true, convert: handleConvert(rateType), csvConvert: true },
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
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '利率端待实现收益·费用',
        bind: 'interestRateRevenueFees',
        align: 'right',
        width: 160,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '利率端待实现收益',
        bind: 'interestRateRevenueSum',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '合约持仓价值',
        bind: 'totalUnRealizedPNL',
        align: 'right',
        width: 120,
        convert: (v) => N2(v),
        csvConvert: true,
        tooltip: true,
      },
      {
        header: '累计已实现盈亏',
        bind: 'totalRealizedPNL',
        align: 'right',
        width: 120,
        convert: (v) => N2(v),
        csvConvert: true,
        tooltip: true,
      },
      { header: '估值更新时间', bind: 'lastEvTime', width: 150, convert: Secformat, csvConvert: true },
    ]
  }, [])
}

function useGetContractColumn(swappaymentType = []) {
  return useCallback(() => {
    return [
      { header: '#', convert: (v, o, i) => i + 1, width: 40, align: 'center', stretch: false },
      { header: '估值日', bind: 'evaluationDate', width: 100, convert: formatDate, csvConvert: true },
      { header: '交易簿', bind: 'tradingBookName', width: 120, tooltip: true },
      { header: '对手方', bind: 'counterpartyName', width: 120, tooltip: true },
      { header: '对手方交易账户', bind: 'tradingAcctName', width: 140, tooltip: true },
      { header: '合约编号', bind: 'contractId', width: 140, tooltip: true },
      { header: '产品类型', bind: 'productType', width: 80, tooltip: true },
      // { header: '券商支付', bind: 'paymentType', width: 80, tooltip: true },
      { header: '起始日', bind: 'startDate', width: 100, convert: formatDate, csvConvert: true },
      { header: '到期日', bind: 'matureDate', width: 100, convert: formatDate, csvConvert: true },
      {
        header: '合约名义本金',
        bind: 'notionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '多头名义本金',
        bind: 'longNotionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '空头名义本金',
        bind: 'shortNotionalPrincipal',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '多头标的市值',
        bind: 'longMarketValue',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '空头标的市值',
        bind: 'shortMarketValue',
        align: 'right',
        width: 100,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '权益端待实现收益',
        bind: 'equityRevenueSum',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      {
        header: '利率端待实现收益',
        bind: 'interestRateRevenueSum',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      { header: '合约持仓价值', bind: 'underlyingMarketValue', width: 120, convert: (v) => N2(v), csvConvert: true, align: 'right' },
      { header: '当日实现收益', bind: 'tdrealized', align: 'right', width: 130, tooltip: true, convert: (v) => N2(v), csvConvert: true }, // 使用的是利率端实现
      {
        header: '合约已实现收益',
        bind: 'totalRealized',
        align: 'right',
        width: 130,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      { header: '估值更新时间', bind: 'lastEvTime', width: 150, convert: Secformat, csvConvert: true },
      {
        header: '对手方初始保证金',
        bind: 'initMarginAm',
        align: 'right',
        width: 120,
        tooltip: true,
        convert: (v) => N2(v),
        csvConvert: true,
      },
      { header: '对手方维持保证金', bind: 'marginAmt', align: 'right', width: 120, tooltip: true, convert: (v) => N2(v), csvConvert: true },
      { header: '维持保证金计算时间', bind: 'marginCalcTime', width: 160, convert: Secformat, csvConvert: true },
      { header: '投资规模', bind: 'investmentScale', convert: (v) => N2(v), align: 'right' },
    ]
  }, [])
}

export function useInitFetchPositon(params, mode) {
  const { data, doFetch, loading, error } = usePost()

  useEffect(() => {
    const fetchUrl = { daytime: `/valuation/subContract/realTime`, reckoning: `/valuation/subContract/search` }
    doFetch(fetchUrl[mode], params)
  }, [doFetch, params, mode])

  return { data, loading, error }
}

export function useInitFetchContract(params, mode) {
  const { data, doFetch, loading, error } = usePost()

  useEffect(() => {
    const fetchUrl = { daytime: `/valuation/contract/realTime`, reckoning: `/valuation/contract/search` }
    doFetch(fetchUrl[mode], params)
  }, [doFetch, params, mode])

  return { data, loading, error }
}

function handleConvert(info) {
  return (value) => info[value] || null
}
