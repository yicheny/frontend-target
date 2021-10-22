import { createSelects } from '../../../../common/utils'
import { createListWithUrl, getDefaultParams } from '../widgetsBiz/Conctract'
import { createTabContent, SContractIds, SCounterPartyIds, STradingBooks } from '../widgets/components'

export default createTabContent({
  List: createListWithUrl('/valuation/contract/realTimeOnParty'),
  Selects: createSelects([STradingBooks, SContractIds, SCounterPartyIds]),
  getDefaultParams,
})
