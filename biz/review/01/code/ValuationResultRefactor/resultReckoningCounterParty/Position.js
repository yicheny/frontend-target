import { createListWithUrl, getDefaultParams } from '../widgetsBiz/Position'
import { createTabContent, SContractLimitIds, SCounterPartyIds, SEvDate, SUnderlyingId } from '../widgets/components'
import { createSelects } from '../../../../common/utils'

export default createTabContent({
  List: createListWithUrl('/valuation/subContract/searchOnParty'),
  Selects: createSelects([SEvDate, SUnderlyingId, SContractLimitIds, SCounterPartyIds]),
  getDefaultParams,
})
