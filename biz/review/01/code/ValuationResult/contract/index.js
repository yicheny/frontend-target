import { useState } from 'react'
import { Button } from 'rootnet-ui'
import { Form, FormInput, Select, DatePicker } from 'rootnet-edit'
import { toDate, dateFormat } from 'rootnet-core/dateFormat'
import { DataChange } from 'rootnet-core/format'
import List from './List'

const { str14ToDate } = toDate
const formatDate = dateFormat('YYYYMMDDHHMMSS')
const evDate = {
  reckoning: (
    <FormInput label='估值日期' component={DatePicker} bind='evDate' bindInConvert={str14ToDate} bindOutConvert={formatDate} clear />
  ),
}

function ContractCard({ selectOption, pageMode }) {
  const { value, setValue, params, setParams, getInitValue } = useInit()

  return (
    <div className='fill flex-y'>
      <div className='c-options'>
        <Form value={value} onChange={setValue}>
          {evDate[pageMode]}
          <FormInput label='交易簿' component={Select} bind='tradingBookIdList' options={selectOption.tradingBook} search clear multiple />
          <FormInput label='合约编号' component={Select} bind='contractIdList' options={selectOption.contractLimit} search clear multiple />
          <FormInput
            label='对手方'
            component={Select}
            bind='counterpartyIdList'
            options={selectOption.counterParty}
            search
            clear
            multiple
          />
        </Form>
        <Button primary onClick={() => setParams((x) => ({ ...x, ...value }))}>
          查询
        </Button>
        <Button className='btn-reset' onClick={handleReset}>
          重置
        </Button>
      </div>
      <List params={params} search={setParams} selectOption={selectOption} pageMode={pageMode} />
    </div>
  )

  function handleReset() {
    setValue(getInitValue())
    setParams(getInitValue())
  }
}

// 增加默认参数，之前点击重置时未能清除筛选项已选中的值（ui层）
function useInit() {
  const getInitValue = () => ({
    evType: 'RealTime',
    evDate: DataChange(new Date()),
    tradingBookIdList: [],
    contractIdList: [],
    counterpartyIdList: [],
  })
  const [value, setValue] = useState(getInitValue())
  const [params, setParams] = useState(getInitValue())

  return { value, setValue, params, setParams, getInitValue }
}

export default ContractCard