import { FormDatePicker, FormSelect } from '../../../../../common/components'
import { useView } from '../../../../../common/context'
import { curryComponent } from '../../../../../common/utils'

const MSFormInput = curryComponent(FormSelect)({ multiple: true })

//估值日期
export function SEvDate() {
  return <FormDatePicker label='估值日期' bind='evDate' clear={false} />
}

//交易簿
export function STradingBooks() {
  return <MSFormInput label='交易簿' bind='tradingBookIdList' options={useView().tradingBookOptions} />
}

//合约编号-contractIdOptions
export function SContractIds() {
  return <MSFormInput label='合约编号' bind='contractIdList' options={useView().contractIdOptions} />
}

//合约编号-contractLimitOptions
export function SContractLimitIds() {
  return <MSFormInput label='合约编号' bind='contractIdList' options={useView().contractLimitOptions} />
}

//对手方
export function SCounterPartyIds() {
  return <MSFormInput label='对手方' bind='counterpartyIdList' options={useView().counterPartyOptions} />
}

//标的
export function SUnderlyingId() {
  return <FormSelect label='标的' bind='underlyingId' options={useView().underlyingOptions} />
}
